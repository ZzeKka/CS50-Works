#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; ++i)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair winner_loser;
    for (int i = 0; i < candidate_count - 1; ++i)
    {
        for (int j = 1; j < candidate_count; ++j)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                winner_loser.winner = i;
                winner_loser.loser = j;
                pairs[pair_count] = winner_loser;
                pair_count++;
            }
            else if (preferences[j][i] > preferences[i][j])
            {
                winner_loser.winner = j;
                winner_loser.loser = i;
                pairs[pair_count] = winner_loser;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair temp;
    int swap;
    int winner_points = 0, winner_points_next = 0;
    for (int i = 0; i < pair_count - 1; ++i)
    {
        swap = 0;
        for (int j = 0; j < pair_count - i - 1; ++j)
        {
            winner_points = preferences[pairs[j].winner][pairs[j].loser];
            winner_points_next = preferences[pairs[j + 1].winner][pairs[j + 1].loser];
            if (winner_points < winner_points_next)
            {
                temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;
                swap = 1;
            }
        }
        if (swap == 0)
        {
            break;
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int cycle;
    int winner, loser;
    int count;
    for (int i = 0; i < pair_count; ++i)
    {
        cycle = 0;
        winner = pairs[i].winner;
        loser = pairs[i].loser;
        count = 0;
        while (true)
        {
            for (int j = 0; j < i; ++j)
            {
                if (winner == pairs[j].loser)
                {
                    winner = pairs[j].winner;
                    if (winner == loser)
                    {
                        cycle = 1;
                        break;
                    }
                }
            }
            count++;
            if (cycle == 1 || count > candidate_count - 1)
            {
                break;
            }
        }
        if (cycle == 0)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // column
    int source;
    for (int j = 0; j < candidate_count; ++j)
    {
        source = 1;
        for (int i = 0; i < candidate_count; ++i)
        {
            if (locked[i][j] == true)
            {
                source = 0;
                break;
            }
        }

        if (source == 1)
        {
            printf("%s\n", candidates[j]);
            return;
        }
    }
    return;
}
