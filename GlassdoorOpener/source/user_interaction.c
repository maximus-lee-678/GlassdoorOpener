#include "../headers/user_interaction.h"

u_int welcome() {
	const char* openers[UNIVERSAL_LENGTH] = {
		"Oh, THESE sunglasses? You can't afford THESE sunglasses. And get your hands off the ropes!",
		"Make sure you get my good side....oh wait! I ONLY HAVE good sides!",
		"When I'm done with you, no one is gonna DARE take your picture!",
		"Are you ready? 'Cause I'm gonna put on a show!"
	};

	srand(time(NULL));
	int random = rand() % 4;

	fprintf(stdout, DIVIDER_50);
	fprintf(stdout, "[~] %s\n", openers[random]);

	char pwd[FILENAME_MAX];
	_getcwd(pwd, FILENAME_MAX);
	if (strstr(pwd, "\\Desktop\\")) {
		fprintf(stderr, "[!] This program does not function on the Desktop directory, I've not a clue why. Move it somewhere else!\n");
		return 1;
	}
	else {
		fprintf(stdout, "[~] Accepted Inputs:\n");
		fprintf(stdout, "[~] Page Number in Digit\n");
		fprintf(stdout, "[~] (Error Handling not implemented don't lame)\n");
		fprintf(stdout, DIVIDER_50);
		fprintf(stdout, "[i] Accepting Inputs\n");
		return 0;
	}
}

int receive_digits() {
	char buffer[UNIVERSAL_LENGTH];				// buffer to hold strings

	fprintf(stdout, "[<] ");

	fgets(buffer, 512, stdin);

	if (strchr(buffer, '\n')) {			// check exist newline
		*strchr(buffer, '\n') = '\0';	// replace newline with null terminator
	}
	else {
		scanf("%*[^\n]");	// clear up to newline
		scanf("%*c");		// clear newline
	}

	return atoi(buffer);
}

void goodbye() {
	fprintf(stdout, DIVIDER_50);
	fprintf(stdout, "[~] Press any key to close this window . . .");
	_getch();
}