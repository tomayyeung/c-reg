#include <stdio.h>

#include "help.h"

void print_help() {
    printf("Usage: creg [-h | --help] <command> [<args>]");
    printf("\nCommands:\n");
    printf("  art       craig\n");
    printf("  login     Login using your USF ID\n");
    printf("  logout    Log out from creg\n");
    printf("  add       Add a section to main or plan\n");
    printf("  rm        Remove a course from main or plan\n");
    printf("  rmplan    Remove a plan and all sections added\n");
    printf("  view      View sections added to main or plan\n");
    printf("  viewplans View a list of all plans\n");
    printf("  browse    Search for classes offered this semester\n");
    printf("  apply     Add all for sections in a plan to main registration\n");
    printf("  cbrowse   Browse catalog for all USF courses\n");
    printf("  schedule  Show weekly schedule of main or plan\n");
}

void print_help_art() {
    printf("Usage: creg art\n");
    printf("  craig\n");
}

void print_help_login() {
    printf("Usage: creg login [username]\n");
    printf("  You will then be prompted for your password.\n");
}

void print_help_logout() {
    printf("Usage: creg logout\n");
}

void print_help_add() {
    printf("Usage: creg add [crn] [-p | --plan=<name>]\n");
    printf("  Adds given crn to specified plan\n");
    printf("  If no plan is specified, adds to main registration\n");
    printf("  If plan does not exist, creates a new plan with the given name\n");
}

void print_help_rm() {
    printf("Usage: creg rm [crn] [-p | --plan=<name>]\n");
    printf("  Removes given crn from specified plan\n");
    printf("  If no plan is specified, removes from main registration\n");
}

void print_help_rmplan() {
    printf("Usage: creg rmplan [name]\n");
    printf("  Removes given plan and all added sections\n");
    printf("  This command is irreversible\n");
}

void print_help_view() {
    printf("Usage: creg view [-p | --plan=<name>]\n");
    printf("  Views all sections aded to specified plan\n");
    printf("  If no plan is specified, views main registration\n");
}

void print_help_viewplans() {
    printf("Usage: creg viewplans\n");
    printf("  Views all created plans\n");
}

void print_help_browse() {
    printf("Usage: creg browse [-s | --subject<subject>] [-n | --number<number>] [-I | --instruction<instrmethod>] \n");
    printf("    [-a | --attributes<attributes>]  [-i | --instructor<name>] [-k | --keywords<keywords>]\n");
    printf("  Browses all sections with given search queries:\n");
    printf("    [-s | --subject<subject>] searches for the given subject, eg CS.\n");
    printf("    [-n | --number<number>] searches for a specified course number, eg 221 for CS 221.\n");
    printf("    [-I | --instruction<instrmethod>] searches for the given instructional method: InPerson, Online, Hybrid.\n");
    printf("    [-a | --attributes<attributes>] to be added\n");
    printf("    [-i | --instructor<name>] searches for the given instructor name. Place the name inside quotes, eg \"Paul Haskell\".\n");
    printf("    [-k | --keywords<keywords>] search for particular keywords\n");
}

void print_help_apply() {
    printf("Usage: creg apply [name]\n");
    printf("  Empties main registration and adds all sections in given plan.\n");
}

void print_help_cbrowse() {
    printf("Usage: creg cbrowse [-s | --subject<subject>] [-n | --number<number>]\n");
    printf("    [-a | --attributes<attributes>] [-k | --keywords<keywords>] [-v | --verbose]\n");
    printf("  Browses all courses (not limited to currently offered sections) with given search queries:\n");
    printf("    [-s | --subject<subject>] searches for the given subject, eg CS.\n");
    printf("    [-n | --number<number>] searches for a specified course number, eg 221 for CS 221.\n");
    printf("    [-a | --attributes<attributes>] to be added\n");
    printf("    [-k | --keywords<keywords>] search for particular keywords\n");
    printf("    [-v | --verbose] gives more information for each course\n");
}

void print_help_schedule() {
    printf("Usage: creg schedule [-p | --plan=<name>]\n");
    printf("  Displays weekly schedule of given plan.\n");
    printf("  Sections are sorted by day and time.\n");
}
