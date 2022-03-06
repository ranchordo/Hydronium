#ifndef SECRETCONFIG
#define SECRETCONFIG

/*
This file is for secret configuration that shouldn't be committed to the repo.
To make sure that git doesn't commit this file, either stage the changes, or
run "git update-index --skip-worktree ./SecretConfig.h" in a terminal.
*/

#define ENV_WIFI_SSID "WIFI SSID HERE"
#define ENV_WIFI_PWD "WIFI PASSWORD HERE"

#endif