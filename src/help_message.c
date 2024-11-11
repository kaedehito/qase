#include <stdio.h>


void help_1(){
	printf("\x1b[2J\x1b[H");
	printf("	Basic operations	\t\t\t\t[PAGE 1]\n");
	printf("================================================================================\n\n");
	printf("To temporarily pause the music, press the s key.\nThis will pause the playback.\nTo resume, press the p key.\n\n");
	printf("Avoid pressing the p key unless the music is paused, as it will display an error message if the music is already playing.\n\n");
	printf("Press the q key to exit qase.\nNote that qase does not respond to forced termination signals, such as Ctrl-C or Ctrl-D.\n\n\n\n");

	printf("← (HOME)\t\t(PAGE 2)→\n");
}

void help_2(){
	printf("\x1b[2J\x1b[H");
	printf("	Volume control		\t\t\t\t[PAGE 2]\n");
	printf("================================================================================\n\n");
	printf("While playing music, you can press the 'j' key to decrease the volume.\n");
	printf("Press the 'k' key to increase the volume.\n");
	printf("\n");
	printf("You can also set the initial volume using the --volume=[VOLUME] option when launching qase.\n");
	printf("\n");
	printf("The maximum volume is 128, and the minimum is 0. If you try to increase or decrease the volume beyond these limits, Qase will display an error message.\n");
	printf("We all dislike errors, right?\n\n\n\n");
	printf("← (PAGE 1)\n");
}

