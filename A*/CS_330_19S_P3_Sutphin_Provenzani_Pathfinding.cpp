#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <math.h>
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include <deque>
#include <climits>
using namespace std;

//**********structs for nodes, routes, and connections**********//
struct connection
{
	int orig;
	int dest;
	int cost;
	void clear()// function to clear the contents of this object
	{
		orig = NULL;
		dest = NULL;
		cost = NULL;
	}
};

struct node
{
	int index;
	int x;
	int y;
	int costSoFar = INT_MAX;
	string name;
	vector<connection> connections; // list of connections
	int heuristic;
	int estTotal = 0;
	int connectBackIndex;
	
	void clear() // function to clear the contents of this object
	{
		x = NULL;
		y = NULL;
		index = NULL;
		heuristic = NULL;
		connectBackIndex = NULL;
		name = "";
		estTotal = INT_MAX;
		for(int i = 0; i < connections.size(); i++)
		{
			connections[i].clear();
		}
	}

	bool operator!=(const node& other) const // overloading the != operator so that i may use it on node objects
	{
		if (other.x == this->x && other.y == this->y && other.index == this->index)
			return false;
		else
			return true;
	}

	bool operator==(const node& other) const // overloading the == operator so that i may use it on node objects
	{
		if (other.x == this->x && other.y == this->y && other.index == this->index)
			return true;
		else
			return false;
	}

};

struct route
{
	int orig;
	int dest;
	void clear()// function to clear the contents of this object
	{
		orig = NULL;
		dest = NULL;
	}
};

//********************************************************//

//**********Function Prototypes**********//

//returns a vector of strrings where each entry is a line of the file
vector<string> readFile(string filename);

//takes in a vector of strings and populates the unvisited list
void initLists(vector<string> file, vector<node> *unvisited, vector<route> *paths);

// ordering function to pass to std::sort 
bool lessThan(const node &one, const node &two);

// The meat and potatoes of this project. What we're all here for.
void A_Star(route path, vector<node> &graph);

//********************************************************//

ofstream outFile;// output stream to use for writing to the output file.

int main()
{
	vector<node> unvisited;
	vector<route> paths;
		
	cout << "Type the name of the file containing the node, connection, and route information:\n";
	string filename;
	cin >> filename;
	cout << "\n\nReading Graph File..." << endl;
	initLists(readFile(filename),&unvisited,&paths);// read the file in and make the unvisited list with all connections as well as make th routes list
	cout << "Done Reading File" << endl;

	outFile.open("CS_330_19S_P3_Sutphin_Provenzani_Results.txt", ofstream::out);
	cout << "\t\tRunning A* On Each Route Given\n" << endl;
	for (int i = 0; i < paths.size(); i++)
	{
		cout << "Route #:" << i+1 << "\nOrigin: " << paths[i].orig << " Destination: " << paths[i].dest << endl;
		cout << "\n\n";
		A_Star(paths[i], unvisited); // run A* for the given route on the given graph
	}
	outFile.close();
	cout << "\t\tDone\n All output is contained in CS_330_19S_P3_Sutphin_Provenzani_Results.txt\n" << endl;
	system("pause");
	return 0;
}


vector<string> readFile(string filename)
{
	vector<string> tempV;
	ifstream inFile;
	string tempS;
	char c;

	inFile.open(filename,ifstream::in);
	//prime inFile to see if good and is_open
	getline(inFile, tempS);
	//discard comments
	if (tempS[0] != '#')
	{
		tempV.push_back(tempS);
	}
	//if the file is open and not corrupted repeat the previous 5 lines until eof
	if (inFile.good() && inFile.is_open())
	{
		while (!inFile.eof())
		{
			getline(inFile, tempS);
			if (tempS[0] != '#' && tempS[0] != '\n')
			{
				tempV.push_back(tempS);
			}

		}
	}
	else
	{
		cout << "Couldn't Open File!!\n";
	}
	inFile.close();
	return tempV;
}

void initLists(vector<string> file, vector<node> *unvisited, vector<route> *paths)
{
	vector<node> tempListUnvis;
	vector<route> tempListRoutes;
	route tempR;
	node tempN;
	connection tempC;

	
	for (int i = 0; i < file.size(); i++)
	{
		stringstream iss(file[i]);// treat each line of text as a stream
		char c;
		iss.get(c); //get the firsh char to see if its a node, connection, or a route.
		if (c == 'N')
		{
			string Line[3];
			int count = 0;
			while (iss.good()) //parse the line of the file into tokens
			{
				string tempLine;
				getline(iss, tempLine, ' ');
				
				if (tempLine != "") // if its actually a char and not NULL add it to our temp array
				{
					Line[count] = tempLine;
					count++;
				}
			}
			tempN.x = stoi(Line[0]); //fill the temp node
			tempN.y = stoi(Line[1]);
			if (Line[2] != "")
				tempN.name = Line[2];
			tempN.index = i + 1;
			tempListUnvis.push_back(tempN); // add it to the list
			tempN.clear();
		}
		else if (c == 'C')
		{
			string Line[3];
			int count = 0;
			while (iss.good()) //parse the line of the file into tokens
			{
				string tempLine;
				getline(iss, tempLine, ' ');

				if (tempLine != "") // if its actually a char and not NULL add it to our temp array
				{
					Line[count] = tempLine;
					count++;
				}
			}
			tempC.orig = stoi(Line[0]); //fill the temp connection
			tempC.dest = stoi(Line[1]);
			tempC.cost = stoi(Line[2]);
			for(int i = 0; i < tempListUnvis.size(); i++)
			{
				if (i + 1 == stoi(Line[0]))
					tempListUnvis[i].connections.push_back(tempC); // add it to the list if the node number is the same as the connection.startingNode
			}
			tempC.clear();
		}
		else if (c == 'R')
		{
			string Line[2];
			int count = 0;
			while (iss.good()) //parse the line of the file into tokens
			{
				string tempLine;
				getline(iss, tempLine, ' ');

				if (tempLine != "") // if its actually a char and not NULL add it to our temp array
				{
					Line[count] = tempLine;
					count++;
				}
			}
			tempR.orig = stoi(Line[0]); //fill the temp route
			tempR.dest = stoi(Line[1]);
			tempListRoutes.push_back(tempR); // add it to the list
			tempR.clear();
		}
		c = '0'; // reset c
	}

	*unvisited = tempListUnvis;// return the graph
	*paths = tempListRoutes;// return the routes we need to run A* on
}


// ordering function for passing to std::sort. 
bool lessThan(const node &one, const node &two)
{
	if (one.estTotal < two.estTotal)
		return true;
	else
		return false;
}

void printTextFile(deque<node> path, int cost)
{

	
	outFile << "\n\n\t\tResults\n";

	if (outFile.good())
	{
		//TODO - find meaningful way to reperesent the path we have found
		
		outFile << "Start Node: " << path[0].index << " (" << path[0].x << "," << path[0].y << ") "  
			<< "Destination Node: " << path[path.size() - 1].index << " (" << path[path.size() -1].x << "," << path[path.size() - 1].y << ")"
			<< " Cost: "  << cost<< endl;
		outFile << "Path Taken" << endl;
		for (int i = 0; i < path.size(); i++)
		{
			if(i != path.size() -1)
				outFile << path[i].index << " ";
			else
				outFile << path[i].index << endl;
		}


	}

}

int heuristic(node n1, node n2)
{
	return sqrt(pow((n2.x * 10) - (n1.x * 10), 2) + pow((n2.y * 10) - (n1.y * 10), 2));
}


void A_Star(route path, vector<node> &graph)
{
	//******begin section*******
	//stuff used for reporting to the text file output
	deque<node> nodesInShortestPath;	
	int cost = 0;					// this is to store the total cost of the final path "nodesInShortestPath" when we are done

			//IMPORTANT
	// after every time you insert a node into the open list use this line of code "sort(open.begin(), open.end(), lessThan);" 
		//to sort it so that to find the node in the open list with the lowest estTotal you just pick open[0]
	//********end Section*******
	vector<node> unvisited = graph;
	vector<node> open;
	vector<node> closed;
	node nodeNULL;
	nodeNULL.clear();

	//TODO - implement A* here

	node currentNode = unvisited[path.orig - 1]; // find starting node given by the path.
	node startNode = currentNode;
	node destNode = unvisited[path.dest - 1];

	currentNode.costSoFar = 0; // Set up its heuristic and cost so far
	currentNode.heuristic = heuristic(currentNode, unvisited[path.dest - 1]);

	open.push_back(currentNode); // add it to the open list
	unvisited.erase(find(unvisited.begin(), unvisited.end(), currentNode));

	while (!open.empty() && currentNode != destNode)
	{
		for (int i = 0; i < currentNode.connections.size(); i++)
		{
			node toNode = nodeNULL; //clear the new node for use
			for (int k = 0; k < unvisited.size();k++)// for all connections of this node...
			{
				if (unvisited[k].index == currentNode.connections[i].dest)
				{
					toNode = unvisited[k];
					unvisited.erase(find(unvisited.begin(), unvisited.end(), toNode)); // remove the unvisited list
				}
			}
			if (toNode == nodeNULL)//if it wasnt in the unvisited list check the open list
			{
				for (int k = 0; k < open.size(); k++)
				{
					if (open[k].index == currentNode.connections[i].dest)
					{
						toNode = open[k];
					}
				}
			}

			int toCost = currentNode.costSoFar + currentNode.connections[i].cost;
			
			if (toNode != nodeNULL)// if we found it
			{

				if (toCost < toNode.costSoFar)
				{
					toNode.costSoFar = toCost;
					toNode.heuristic = heuristic(toNode, destNode);
					toNode.estTotal = toNode.costSoFar + toNode.heuristic;
					toNode.connectBackIndex = currentNode.index;
					bool inList = false;
					for (int l = 0; l < open.size(); l++)// see if we have already pushed it tp the open list
					{
						if (open[l] == toNode)
						{
							inList = true;
						}
					}

					if (!inList)//push it to the open list as long as its not already in there
					{
						open.push_back(toNode);
					}

				}

			}

		}

		
		bool inList = false;
		for (int l = 0; l < closed.size(); l++)// see if e have already pushed it to the closed list
		{
			if (closed[l] == currentNode)
			{
				inList = true;
			}
		}

		if (!inList)
		{
			closed.push_back(currentNode); // add current node to the closed list if its not already there
		}


		open.erase(find(open.begin(), open.end(), currentNode)); // remove the open list
		std::sort(open.begin(), open.end(), lessThan); //sort the open list by lowest estTotal Lowest to highest
		
		currentNode = open[0];

		if (currentNode == destNode)// if we found the destination then we're done.
			closed.push_back(currentNode);
	}
	for (int k = 0; k < closed.size(); k++)// calculate the totl cost and push the closed list to a list that can be sent to a txt file.
	{
		nodesInShortestPath.push_back(closed[k]);
		cost += closed[k].costSoFar;
	}

	printTextFile(nodesInShortestPath, cost);// print our path
}