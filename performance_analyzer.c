static void displayRoleAcrossTeams(void)
{
    int role;
    if (!readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ",
                 1, 3, &role)) return;

    HeapNode heap[TEAM_COUNT];
    int heapSize = 0;

    for (int t = 0; t < TEAM_COUNT; t++) {
        PlayerNode *head = teamRecords[t].rankHeads[role - 1];
        if (head) {
            HeapNode item = {head, t};
            heapPush(heap, &heapSize, item);
        }
    }

    printf("\n%s across all teams (descending performance index)\n",
           roleNames[role - 1]);
    printHeader(1);

    
    while (heapSize > 0) {
        HeapNode item = heapPop(heap, &heapSize);
        printPlayer(&item.player->data, 1);

        PlayerNode *next = item.player->nextRank;
        if (next) {
            HeapNode replacement = {next, item.teamIndex};
            heapPush(heap, &heapSize, replacement);
        }
    }
}

static void printMenu(void)
{
    puts("\n==============================================================");
    puts("             ICC ODI Player Performance Analyzer");
    puts("==============================================================");
    puts("1. Add Player to Team");
    puts("2. Display Players of a Specific Team");
    puts("3. Display Teams by Average Batting Strike Rate");
    puts("4. Display Top K Players of a Specific Team by Role");
    puts("5. Display All Players of a Specific Role Across All Teams");
    puts("6. Exit");
    puts("==============================================================");
}

static void freeAllPlayers(void)
{
    for (int t = 0; t < TEAM_COUNT; t++) {
        PlayerNode *current = teamRecords[t].players;
        while (current) {
            PlayerNode *next = current->nextTeam;
            free(current);
            current = next;
        }
        teamRecords[t].players = NULL;
        for (int r = 0; r < ROLE_COUNT; r++)
            teamRecords[t].rankHeads[r] = NULL;
    }
}

int main(void)
{
    int choice;

    
    for (int i = 0; i < TEAM_COUNT; i++) {
        teamRecords[i].teamId = i + 1;
        snprintf(teamRecords[i].name, sizeof teamRecords[i].name, "%s", teams[i]);
    }

    initializeData();
    printf("Loaded %d players across %d teams.\n", totalPlayers, TEAM_COUNT);
    puts("Team IDs follow the order in Players_data.h:");
    for (int i = 0; i < TEAM_COUNT; i++)
        printf("%d. %s\n", teamRecords[i].teamId, teamRecords[i].name);

    for (;;) {
        printMenu();
        if (!readInt("Enter your choice: ", 1, 6, &choice)) break;

        switch (choice) {
            case 1: addPlayer(); break;
            case 2: displayTeamPlayers(); break;
            case 3: displayTeamsByStrikeRate(); break;
            case 4: displayTopK(); break;
            case 5: displayRoleAcrossTeams(); break;
            case 6:
                freeAllPlayers();
                puts("Exiting. Memory released.");
                return 0;
            default: break;
        }
    }

    freeAllPlayers();
    return 0;
}
