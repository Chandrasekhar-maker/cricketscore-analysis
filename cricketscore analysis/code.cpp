#include <iostream>
#include <string>
using namespace std;

const int MAX_PLAYERS = 11;

class Batsman
{
public:
    string name;
    int runs;
    int balls;
    int fours;
    int sixes;
    bool out;

    Batsman()
    {
        name = "";
        runs = 0;
        balls = 0;
        fours = 0;
        sixes = 0;
        out = false;
    }

    void addRuns(int r)
    {
        runs += r;
        balls++;

        if (r == 4)
            fours++;
        else if (r == 6)
            sixes++;
    }

    void display()
    {
        cout << name << "\t" << runs << "\t" << balls
             << "\t" << fours << "\t" << sixes << endl;
    }
};

class Bowler
{
public:
    string name;
    int ballsBowled;
    int runsGiven;
    int wickets;

    Bowler()
    {
        name = "";
        ballsBowled = 0;
        runsGiven = 0;
        wickets = 0;
    }

    void addBall()
    {
        ballsBowled++;
    }

    void display()
    {
        cout << name << "\t"
             << ballsBowled / 6 << "."
             << ballsBowled % 6 << "\t"
             << runsGiven << "\t"
             << wickets << endl;
    }
};

class Team
{
public:
    string name;
    Batsman players[MAX_PLAYERS];
    int totalRuns;
    int totalWickets;
    int extras;

    Team()
    {
        totalRuns = 0;
        totalWickets = 0;
        extras = 0;
    }

    void inputTeam()
    {
        cout << "Enter team name: ";
        cin >> name;

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            cout << "Enter player " << i + 1 << " name: ";
            cin >> players[i].name;
        }
    }

    void displayScore()
    {
        cout << "\n------ " << name << " SCOREBOARD ------\n";
        cout << "Score: " << totalRuns << "/" << totalWickets << endl;

        cout << "\nBatter\tRuns\tBalls\t4s\t6s\n";

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (players[i].name != "")
                players[i].display();
        }

        cout << "Extras: " << extras << endl;
    }
};

class Match
{
private:
    Team team1, team2;

    Team *battingTeam;        
    Team *bowlingTeam;         

    Bowler bowlers[MAX_PLAYERS];
    int currentBowler;          
    int previousBowler;          
    int maxOversPerBowler;      

    int overs;
    int striker;
    int nonStriker;
    int nextBatsman;

    int findPlayer(Team &t, string n)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (t.players[i].name == n)
                return i;
        }
        return -1;
    }

    void doToss()
    {
        int tossWinner, decision;

        cout << "\n------ TOSS ------\n";

        while (true)
        {
            cout << "Who won the toss?\n";
            cout << "1. " << team1.name << "\n";
            cout << "2. " << team2.name << "\n";
            cout << "Enter choice: ";
            cin >> tossWinner;

            if (tossWinner == 1 || tossWinner == 2)
                break;

            cout << "Invalid choice. Try again.\n";
        }

        string winnerName = (tossWinner == 1) ? team1.name : team2.name;

        while (true)
        {
            cout << "\n" << winnerName << " won the toss. Choose:\n";
            cout << "1. Bat first\n";
            cout << "2. Bowl first\n";
            cout << "Enter choice: ";
            cin >> decision;

            if (decision == 1 || decision == 2)
                break;

            cout << "Invalid choice. Try again.\n";
        }

        bool team1Bats;
        if (tossWinner == 1 && decision == 1)
            team1Bats = true;
        else if (tossWinner == 2 && decision == 2)
            team1Bats = true;
        else
            team1Bats = false;

        if (team1Bats)
        {
            battingTeam = &team1;
            bowlingTeam = &team2;
        }
        else
        {
            battingTeam = &team2;
            bowlingTeam = &team1;
        }

        cout << "\n" << battingTeam->name << " will bat first.\n";
        cout << bowlingTeam->name << " will bowl first.\n";
    }

    void chooseBowler(int overNo)
    {
        while (true)
        {
            string n;
            cout << "\nOver " << overNo << " - Enter bowler name (from "
                 << bowlingTeam->name << "): ";
            cin >> n;

            int idx = findPlayer(*bowlingTeam, n);

            if (idx == -1)
            {
                cout << "Invalid! Bowler must be a player of " << bowlingTeam->name << ".\n";
            }
            else if (idx == previousBowler)
            {
                cout << "Invalid! A bowler cannot bowl two overs in a row.\n";
            }
            else if (bowlers[idx].ballsBowled / 6 >= maxOversPerBowler)
            {
                cout << "Invalid! " << n << " has already bowled the maximum "
                     << maxOversPerBowler << " over(s).\n";
            }
            else
            {
                currentBowler = idx;
                bowlers[idx].name = n;
                break;
            }
        }
    }

public:
    void startMatch()
    {
        cout << "Enter number of overs: ";
        cin >> overs;

        maxOversPerBowler = (overs + 4) / 5;

        cout << "\nEnter details for Team 1\n";
        team1.inputTeam();

        cout << "\nEnter details for Team 2\n";
        team2.inputTeam();

        doToss();

        striker = 0;
        nonStriker = 1;
        nextBatsman = 2;

        currentBowler = -1;
        previousBowler = -1;

        playInnings();
    }

    void playInnings()
    {
        Team &batting = *battingTeam;   

        int totalBalls = overs * 6;
        int lastOverAsked = 0;        

        for (int ball = 1; ball <= totalBalls; )
        {
            if (batting.totalWickets == 10)
                break;

            int overNo = (ball - 1) / 6 + 1;

            if (overNo != lastOverAsked)
            {
                if (overNo > 1)
                {
                    previousBowler = currentBowler;

                    int temp = striker;
                    striker = nonStriker;
                    nonStriker = temp;
                }

                chooseBowler(overNo);
                lastOverAsked = overNo;
            }

            Bowler &bowler = bowlers[currentBowler];   

            cout << "\nBall " << ball << " (Over " << overNo << ", bowler: "
                 << bowler.name << ")" << endl;
            cout << "1. Dot ball\n";
            cout << "2. Runs (1, 2, 3, 4, 6)\n";
            cout << "3. Wicket\n";
            cout << "4. Wide\n";
            cout << "5. No-ball\n";

            int choice;
            cout << "Enter event: ";
            cin >> choice;

            if (choice == 1)
            {
                batting.players[striker].balls++;
                bowler.addBall();
                ball++;
            }
            else if (choice == 2)
            {
                int runs;
                cout << "Enter runs: ";
                cin >> runs;

                if (runs == 1 || runs == 2 || runs == 3 ||
                    runs == 4 || runs == 6)
                {
                    batting.players[striker].addRuns(runs);
                    batting.totalRuns += runs;
                    bowler.runsGiven += runs;

                    if (runs % 2 == 1)
                    {
                        int temp = striker;
                        striker = nonStriker;
                        nonStriker = temp;
                    }

                    bowler.addBall();
                    ball++;
                }
                else
                {
                    cout << "Invalid runs. Try again.\n";
                }
            }
            else if (choice == 3)
            {
                batting.players[striker].balls++;
                batting.players[striker].out = true;
                batting.totalWickets++;
                bowler.wickets++;
                bowler.addBall();
                ball++;

                if (nextBatsman < MAX_PLAYERS)
                {
                    striker = nextBatsman;
                    nextBatsman++;
                }
            }
            else if (choice == 4)
            {
                batting.totalRuns++;
                batting.extras++;
                bowler.runsGiven++;
            }
            else if (choice == 5)
            {
                int runs;
                cout << "Enter runs scored off the bat: ";
                cin >> runs;

                batting.totalRuns += runs + 1;
                batting.extras++;
                bowler.runsGiven += runs + 1;
            }
            else
            {
                cout << "Invalid choice. Try again.\n";
            }

            batting.displayScore();

            cout << "\nBowling Figures\n";
            cout << "Bowler\tOvers\tRuns\tWickets\n";
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (bowlers[i].name != "")
                    bowlers[i].display();
            }
        }

        cout << "\nInnings completed!\n";
        batting.displayScore();

        cout << "\nFinal Bowling Figures\n";
        cout << "Bowler\tOvers\tRuns\tWickets\n";
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (bowlers[i].name != "")
                bowlers[i].display();
        }
    }
};

int main()
{
    Match match;
    match.startMatch();

    return 0;
}
