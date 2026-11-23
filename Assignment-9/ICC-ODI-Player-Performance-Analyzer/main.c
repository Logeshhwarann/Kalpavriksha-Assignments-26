#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_NAME 51
#define ROLE_COUNT 3

typedef struct
{
    int PlayerId;
    char Name[MAX_NAME];
    char TeamName[MAX_NAME];
    int Role;
    int TotalRuns;
    float BattingAverage;
    float StrikeRate;
    int Wickets;
    float EconomyRate;
    float PerformanceIndex;
} ODIPlayer;

typedef struct PlayerNode
{
    ODIPlayer player;
    struct PlayerNode *nextAll;
    struct PlayerNode *nextRole;
} PlayerNode;

typedef struct
{
    int TeamId;
    char Name[MAX_NAME];
    int TotalPlayers;
    float AverageBattingStrikeRate;
    PlayerNode *allPlayers;
    PlayerNode *roleHead[ROLE_COUNT];
} Team;

typedef struct
{
    PlayerNode *node;
    int teamIndex;
} HeapNode;

Team teamList[TEAM_COUNT];
int totalPlayers = 0;

float calculatePerformance(ODIPlayer player)
{
    if (player.Role == 1)
        return (player.BattingAverage * player.StrikeRate) / 100.0f;
    if (player.Role == 2)
        return (player.Wickets * 2.0f) + (100.0f - player.EconomyRate);
    return ((player.BattingAverage * player.StrikeRate) / 100.0f) + (player.Wickets * 2.0f);
}

int getRole(char role[])
{
    if (strcmp(role, "Batsman") == 0)
        return 1;
    if (strcmp(role, "Bowler") == 0)
        return 2;
    if (strcmp(role, "All-rounder") == 0)
        return 3;
    return 0;
}

char *getRoleName(int role)
{
    if (role == 1)
        return "Batsman";
    if (role == 2)
        return "Bowler";
    return "All-rounder";
}

void initializeTeams()
{
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        teamList[teamNumber].TeamId = teamNumber + 1;
        strcpy(teamList[teamNumber].Name, teams[teamNumber]);
        teamList[teamNumber].TotalPlayers = 0;
        teamList[teamNumber].AverageBattingStrikeRate = 0.0f;
        teamList[teamNumber].allPlayers = NULL;

        for (int roleNumber = 0; roleNumber < ROLE_COUNT; roleNumber++)
            teamList[teamNumber].roleHead[roleNumber] = NULL;
    }
}

int searchTeam(int teamId)
{
    int low = 0;
    int high = TEAM_COUNT - 1;
    while (low <= high)
    {
        int middle = low + (high - low) / 2;
        if (teamList[middle].TeamId == teamId)
            return middle;
        if (teamList[middle].TeamId < teamId)
            low = middle + 1;
        else
            high = middle - 1;
    }
    return -1;
}

int playerExists(int playerId)
{
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        PlayerNode *current = teamList[teamNumber].allPlayers;
        while (current != NULL)
        {
            if (current->player.PlayerId == playerId)
                return 1;
            current = current->nextAll;
        }
    }
    return 0;
}

void insertPlayer(PlayerNode *newNode, int teamIndex)
{
    Team *team = &teamList[teamIndex];
    newNode->nextAll = team->allPlayers;
    team->allPlayers = newNode;
    int roleIndex = newNode->player.Role - 1;
    PlayerNode **head = &team->roleHead[roleIndex];
    if (*head == NULL || (*head)->player.PerformanceIndex < newNode->player.PerformanceIndex)
    {
        newNode->nextRole = *head;
        *head = newNode;
    }
    else
    {
        PlayerNode *current = *head;
        while (current->nextRole != NULL &&
               current->nextRole->player.PerformanceIndex >= newNode->player.PerformanceIndex)
        {
            current = current->nextRole;
        }
        newNode->nextRole = current->nextRole;
        current->nextRole = newNode;
    }
    team->TotalPlayers++;
    totalPlayers++;
}

void initializePlayers()
{
    for (int playerNumber = 0; playerNumber < playerCount; playerNumber++)
    {
        ODIPlayer player;
        player.PlayerId = players[playerNumber].id;
        strcpy(player.Name, players[playerNumber].name);
        strcpy(player.TeamName, players[playerNumber].team);
        player.Role = getRole((char *)players[playerNumber].role);
        player.TotalRuns = players[playerNumber].totalRuns;
        player.BattingAverage = players[playerNumber].battingAverage;
        player.StrikeRate = players[playerNumber].strikeRate;
        player.Wickets = players[playerNumber].wickets;
        player.EconomyRate = players[playerNumber].economyRate;
        player.PerformanceIndex = calculatePerformance(player);
        int teamIndex = -1;
        for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
        {
            if (strcmp(teamList[teamNumber].Name, player.TeamName) == 0)
            {
                teamIndex = teamNumber;
                break;
            }
        }
        if (teamIndex != -1)
        {
            PlayerNode *newNode = malloc(sizeof(PlayerNode));
            if (newNode == NULL)
                exit(1);
            newNode->player = player;
            newNode->nextAll = NULL;
            newNode->nextRole = NULL;
            insertPlayer(newNode, teamIndex);
        }
    }
}

void calculateTeamAverage(int teamIndex)
{
    PlayerNode *current = teamList[teamIndex].allPlayers;
    float total = 0.0f;
    int count = 0;
    while (current != NULL)
    {
        if (current->player.Role == 1 || current->player.Role == 3)
        {
            total += current->player.StrikeRate;
            count++;
        }
        current = current->nextAll;
    }
    if (count > 0)
        teamList[teamIndex].AverageBattingStrikeRate = total / count;
    else
        teamList[teamIndex].AverageBattingStrikeRate = 0.0f;
}

void addPlayer()
{
    int teamId;
    printf("Enter Team ID to add player: ");
    scanf("%d", &teamId);
    int teamIndex = searchTeam(teamId);
    if (teamIndex == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }
    ODIPlayer player;
    printf("Enter Player Details:\n");
    printf("Player ID: ");
    scanf("%d", &player.PlayerId);
    if (player.PlayerId < 1 || player.PlayerId > 1000 || playerExists(player.PlayerId))
    {
        printf("Invalid or duplicate Player ID!\n");
        return;
    }
    printf("Name: ");
    scanf(" %50[^\n]", player.Name);
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &player.Role);
    if (player.Role < 1 || player.Role > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    printf("Total Runs: ");
    scanf("%d", &player.TotalRuns);
    printf("Batting Average: ");
    scanf("%f", &player.BattingAverage);
    printf("Strike Rate: ");
    scanf("%f", &player.StrikeRate);
    printf("Wickets: ");
    scanf("%d", &player.Wickets);
    printf("Economy Rate: ");
    scanf("%f", &player.EconomyRate);
    strcpy(player.TeamName, teamList[teamIndex].Name);
    player.PerformanceIndex = calculatePerformance(player);
    PlayerNode *newNode = malloc(sizeof(PlayerNode));
    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    newNode->player = player;
    newNode->nextAll = NULL;
    newNode->nextRole = NULL;
    insertPlayer(newNode, teamIndex);
    printf("Player added successfully to Team %s!\n", teamList[teamIndex].Name);
}

void printPlayerHeader()
{
    printf("====================================================================================\n");
    printf("%-6s %-25s %-13s %-8s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");
}

void printPlayer(ODIPlayer player)
{
    printf("%-6d %-25s %-13s %-8d %-8.2f %-8.2f %-8d %-8.2f %-10.2f\n",
           player.PlayerId, player.Name, getRoleName(player.Role),
           player.TotalRuns, player.BattingAverage, player.StrikeRate,
           player.Wickets, player.EconomyRate, player.PerformanceIndex);
}

void displayTeamPlayers()
{
    int teamId;
    printf("Enter Team ID: ");
    scanf("%d", &teamId);
    int teamIndex = searchTeam(teamId);
    if (teamIndex == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }
    calculateTeamAverage(teamIndex);
    printf("Players of Team %s:\n", teamList[teamIndex].Name);
    printPlayerHeader();
    PlayerNode *current = teamList[teamIndex].allPlayers;
    while (current != NULL)
    {
        printPlayer(current->player);
        current = current->nextAll;
    }
    printf("====================================================================================\n");
    printf("Total Players: %d\n", teamList[teamIndex].TotalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamList[teamIndex].AverageBattingStrikeRate);
}

void displayTeamsByStrikeRate()
{
    Team sortedTeams[TEAM_COUNT];
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        calculateTeamAverage(teamNumber);
        sortedTeams[teamNumber] = teamList[teamNumber];
    }
    for (int sortPass = 0; sortPass < TEAM_COUNT - 1; sortPass++)
    {
        for (int sortPosition = 0; sortPosition < TEAM_COUNT - sortPass - 1; sortPosition++)
        {
            if (sortedTeams[sortPosition].AverageBattingStrikeRate <
                sortedTeams[sortPosition + 1].AverageBattingStrikeRate)
            {
                Team temp = sortedTeams[sortPosition];
                sortedTeams[sortPosition] = sortedTeams[sortPosition + 1];
                sortedTeams[sortPosition + 1] = temp;
            }
        }
    }
    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-6s %-20s %-15s %-10s\n", "ID", "Team Name", "Avg Bat SR", "Players");
    printf("=========================================================\n");
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        printf("%-6d %-20s %-15.2f %-10d\n",
               sortedTeams[teamNumber].TeamId,
               sortedTeams[teamNumber].Name,
               sortedTeams[teamNumber].AverageBattingStrikeRate,
               sortedTeams[teamNumber].TotalPlayers);
    }
    printf("=========================================================\n");
}

void displayTopKPlayers()
{
    int teamId;
    int role;
    int k;
    printf("Enter Team ID: ");
    scanf("%d", &teamId);
    int teamIndex = searchTeam(teamId);
    if (teamIndex == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role < 1 || role > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    printf("Enter number of players K: ");
    scanf("%d", &k);
    if (k <= 0)
    {
        printf("Invalid K!\n");
        return;
    }
    printf("Top %d %s players of Team %s:\n",k, getRoleName(role), teamList[teamIndex].Name);
    printPlayerHeader();
    PlayerNode *current = teamList[teamIndex].roleHead[role - 1];
    int count = 0;
    while (current != NULL && count < k)
    {
        printPlayer(current->player);
        current = current->nextRole;
        count++;
    }
    if (count == 0)
        printf("No players found for this role.\n");
    printf("====================================================================================\n");
}

void heapify(HeapNode heap[], int size, int index)
{
    int largest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    if (leftChild < size &&
        heap[leftChild].node->player.PerformanceIndex >
        heap[largest].node->player.PerformanceIndex)
        largest = leftChild;
    if (rightChild < size &&
        heap[rightChild].node->player.PerformanceIndex >
        heap[largest].node->player.PerformanceIndex)
        largest = rightChild;
    if (largest != index)
    {
        HeapNode temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;
        heapify(heap, size, largest);
    }
}

void insertHeap(HeapNode heap[], int *size, HeapNode value)
{
    int heapPosition = *size;
    heap[heapPosition] = value;
    (*size)++;
    while (heapPosition > 0)
    {
        int parentPosition = (heapPosition - 1) / 2;
        if (heap[parentPosition].node->player.PerformanceIndex >=
            heap[heapPosition].node->player.PerformanceIndex)
            break;
        HeapNode temp = heap[parentPosition];
        heap[parentPosition] = heap[heapPosition];
        heap[heapPosition] = temp;
        heapPosition = parentPosition;
    }
}

HeapNode removeHeap(HeapNode heap[], int *size)
{
    HeapNode result = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    if (*size > 0)
        heapify(heap, *size, 0);
    return result;
}

void displayAllPlayersByRole()
{
    int role;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);
    if (role < 1 || role > 3)
    {
        printf("Invalid role!\n");
        return;
    }
    HeapNode heap[TEAM_COUNT];
    int heapSize = 0;
    int roleIndex = role - 1;
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        if (teamList[teamNumber].roleHead[roleIndex] != NULL)
        {
            HeapNode value;
            value.node = teamList[teamNumber].roleHead[roleIndex];
            value.teamIndex = teamNumber;
            insertHeap(heap, &heapSize, value);
        }
    }
    printf("All %s players of all teams:\n", getRoleName(role));
    printf("======================================================================================\n");
    printf("%-6s %-25s %-17s %-13s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "Perf.Index");
    printf("======================================================================================\n");
    int count = 0;
    while (heapSize > 0)
    {
        HeapNode current = removeHeap(heap, &heapSize);
        ODIPlayer player = current.node->player;
        printf("%-6d %-25s %-17s %-13s %-8d %-8.2f %-8.2f %-8d %-10.2f\n",
               player.PlayerId, player.Name, player.TeamName,
               getRoleName(player.Role), player.TotalRuns,
               player.BattingAverage, player.StrikeRate,
               player.Wickets, player.PerformanceIndex);
        count++;
        if (current.node->nextRole != NULL)
        {
            HeapNode next;
            next.node = current.node->nextRole;
            next.teamIndex = current.teamIndex;
            insertHeap(heap, &heapSize, next);
        }
    }
    printf("======================================================================================\n");
    printf("Total Players: %d\n", count);
}

void freeMemory()
{
    for (int teamNumber = 0; teamNumber < TEAM_COUNT; teamNumber++)
    {
        PlayerNode *current = teamList[teamNumber].allPlayers;
        while (current != NULL)
        {
            PlayerNode *nextNode = current->nextAll;
            free(current);
            current = nextNode;
        }
        teamList[teamNumber].allPlayers = NULL;
        for (int roleNumber = 0; roleNumber < ROLE_COUNT; roleNumber++)
            teamList[teamNumber].roleHead[roleNumber] = NULL;
    }
}

int main()
{
    initializeTeams();
    initializePlayers();
    int choice;
    while (1)
    {
        printf("Menu:\n");
        printf("==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            printf("Invalid input!\n");
            continue;
        }
        switch (choice)
        {
            case 1:
                addPlayer();
                break;
            case 2:
                displayTeamPlayers();
                break;
            case 3:
                displayTeamsByStrikeRate();
                break;
            case 4:
                displayTopKPlayers();
                break;
            case 5:
                displayAllPlayersByRole();
                break;
            case 6:
                freeMemory();
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}