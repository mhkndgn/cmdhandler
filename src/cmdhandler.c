#include <config.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>

#include <librdkafka/rdkafka.h>

#include "defines.h"
#include "cfg.h"
#include "globals.h"
#include "utils.h"


char config_file[CONFIG_FILE_LEN];

static volatile sig_atomic_t run = 1;
static rd_kafka_t *rk;
int show_delivery_messages = 1;

static void msg_delivered (rd_kafka_t *rk,
                           const rd_kafka_message_t *rkmessage, void *opaque) {
        if (rkmessage->err)
		fprintf(stderr, "%% Message delivery failed: %s\n",
                        rd_kafka_err2str(rkmessage->err));
	else if (show_delivery_messages)
		fprintf(stdout,
                        "%% Message delivered (%zd bytes, offset %"PRId64", "
                        "partition %"PRId32"): %.*s\n",
                        rkmessage->len, rkmessage->offset,
			rkmessage->partition,
			(int)rkmessage->len, (const char *)rkmessage->payload);
}

void execute_command(char *command, char outputs[MAX_CMD_OUTPUT_LINE_NUM][MAX_CMD_OUTPUT_LINE_LEN], int *lines){
	FILE *fpipe;
	int line_num = 0;
	char line[MAX_CMD_OUTPUT_LINE_LEN];
	
	
	if ( !(fpipe = (FILE*)popen(command,"r")) )
	{  // If fpipe is NULL
		perror("Problems with pipe");
		exit(1);
	}
	
	while(fgets(line, MAX_CMD_OUTPUT_LINE_LEN, fpipe) != NULL){
		strncpy(outputs[line_num], line,MAX_CMD_OUTPUT_LINE_LEN);
		line_num++;
	}
	
	*lines = line_num;
	
	pclose(fpipe);
}

void usage(void){
	
	printf("  -f  \t config_file\n");
}


int main (int argc, char **argv)
{
	rd_kafka_t *rk;
	rd_kafka_topic_t *rkt;
	int partition = RD_KAFKA_PARTITION_UA;
	int opt;
	rd_kafka_conf_t *conf;
	rd_kafka_topic_conf_t *topic_conf;
	char errstr[512];
	char tmp[16];
	int get_wmarks = 0;
        rd_kafka_resp_err_t err;
	struct timeval stop, start;

	while((opt = getopt(argc, argv, ARGS_COMMANDD)) != -1) {	
		switch (opt) {
		case 'f':
			strncpy(config_file, optarg, sizeof(config_file));
			break;
		case 'U':
			usage();
			exit(1);
			break;
		default:
			usage();
			exit(1);
			break;
		}
	}
	
	if (strlen(config_file)) {
		if (parse_configuration_file(config_file) != SUCCESS){
			fprintf(stderr,"cannot parse config file!! exiting... \n");
			exit(1);
		}
	}
	
	/* Kafka configuration */
	conf = rd_kafka_conf_new();

	/* Quick termination */
	snprintf(tmp, sizeof(tmp), "%i", SIGIO);
	rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);
	
	/* Topic configuration */
	topic_conf = rd_kafka_topic_conf_new();

	/*
	 * Producer
	 */
	
	/* Set up a message delivery report callback.
	 * It will be called once for each message, either on successful
	 * delivery to broker, or upon failure to deliver to broker. */
         rd_kafka_conf_set_dr_msg_cb(conf, msg_delivered);
	
	/* Create Kafka handle */
	if (!(rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf,
				errstr, sizeof(errstr)))) {
		fprintf(stderr,
			"%% Failed to create new producer: %s\n",
			errstr);
		exit(1);
	}
	
	/* Add broker */ 
	if (rd_kafka_brokers_add(rk, config.kafka_host) == 0) {
		fprintf(stderr, "%% No valid brokers specified\n");
		exit(1);
	}

	/* Create topic */
	rkt = rd_kafka_topic_new(rk, config.kafka_topic, topic_conf);
	gettimeofday(&start, NULL);
	
	while(TRUE){

		gettimeofday(&stop, NULL);
		if(stop.tv_usec < (start.tv_usec + (suseconds_t)SEC2USEC(config.time_interval)))
			sleep(config.time_interval);
		
		for(int i=0; i<cmd_num; i++){ //iterate over commands
		
			err = RD_KAFKA_RESP_ERR_NO_ERROR;
			char outputs[MAX_CMD_OUTPUT_LINE_NUM][MAX_CMD_OUTPUT_LINE_LEN];
			int output_lines;
			
			execute_command(commands[i].cmd,outputs,&output_lines);

			for(int j=0; j<output_lines; j++){ // iterate over output lines

				char *json_str = NULL;
				json_t *obj = json_object();

				compose_json_uuid(obj,config.cpe_uuid);
		
				if(commands[i].cmd_fields_num > 1){
					parse_output_with_fields(obj, outputs[j], commands[i].cmd_fields, commands[i].cmd_fields_num);
				}
				else{
					parse_output(obj, commands[i].cmd_fields[0], outputs[j]);
				}
				
				compose_json_timestamp(obj);
			
				json_str = compose_json_str(obj);

				/* Send/Produce message. */
		                    
		                    if (rd_kafka_produce(
		                                rkt, partition,
		                                RD_KAFKA_MSG_F_COPY,
		                                /* Payload and length */
		                                json_str, strlen(json_str),
		                                /* Optional key and its length */
		                                NULL, 0,
		                                /* Message opaque, provided in
		                                 * delivery report callback as
		                                 * msg_opaque. */
		                                NULL) == -1) {
		                            err = rd_kafka_last_error();
		                    }

		                    if (err) {
		                            fprintf(stderr,
		                                    "%% Failed to produce to topic %s "
						"partition %i: %s\n",
						rd_kafka_topic_name(rkt), partition,
						rd_kafka_err2str(err));

					/* Poll to handle delivery reports */
					rd_kafka_poll(rk, 0);
					continue;
				}
				/* Poll to handle delivery reports */
				rd_kafka_poll(rk, 0);
				
			}
	    	}

		gettimeofday(&start, NULL);
	} 

	/* Poll to handle delivery reports */
	rd_kafka_poll(rk, 0);

	/* Wait for messages to be delivered */
	while (run && rd_kafka_outq_len(rk) > 0)
		rd_kafka_poll(rk, 100);

	/* Destroy topic */
	rd_kafka_topic_destroy(rkt);

	/* Destroy the handle */
	rd_kafka_destroy(rk);
	
  return 0;
}
