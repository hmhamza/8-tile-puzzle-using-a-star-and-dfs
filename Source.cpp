#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include <time.h>

#include <windows.h> 
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

using namespace std;

string INPUT_GRID[10] = { " _________________ ", "|     |     |     |", "|     |     |     |", "|_____|_____|_____|", "|     |     |     |", "|     |     |     |", "|_____|_____|_____|", "|     |     |     |", "|     |     |     |", "|_____|_____|_____|" };

int Original[3][3] = { { 0, 8, 7 }, { 6, 5, 4 }, { 3, 2, 1 } };

int Total_States = 0;
int States_Explored = 0;

void PlaceCursor(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(h, c);
}

int HEURISTIC(const int arr[][3]){

	int sum = 0, x, y;
	for (int i = 0; i < 3; i++){			//Manhattan
		for (int j = 0; j < 3; j++){
			if (arr[i][j] != 0){		//If not white space
				for (int a = 0; a < 3; a++){
					for (int b = 0; b < 3; b++){
						if (arr[i][j] == Original[a][b]){
							sum += abs(i - a) + abs(j - b);
							break;
						}
						if (arr[i][j] == Original[a][b]){
							break;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < 3; i++){			//Hamming
		for (int j = 0; j < 3; j++){
			if (arr[i][j] != Original[i][j])
				sum++;
		}

	}

	return sum;
}

void _INPUT(int arr[][3]){

	cout << "Enter the Initial configuration of the board\n(Enter any number from 1-8 once and 0 for space)\n\n";
	for (int i = 0; i < 10; i++)
		cout << INPUT_GRID[i] << endl;

	for (int i = 0, y = 5; i < 3; i++, y += 3){
		for (int j = 0, x = 3; j < 3; j++, x += 6){
			PlaceCursor(x, y);
			cin >> arr[i][j];			
		}
	}
	PlaceCursor(0, 15);
	system("pause");
	system("cls");
}

void OUTPUT(const int arr[][3], ofstream &fout){

	string Grid[10];

	for (int i = 0; i < 10; i++)
		Grid[i] = INPUT_GRID[i];

	for (int i = 0, y = 2; i < 3; i++, y += 3){
		for (int j = 0, x = 3; j < 3; j++, x += 6){
			if (arr[i][j] != 0)
				Grid[y].at(x) = arr[i][j] + 48;
		}
	}

	for (int i = 0; i < 10; i++)
		fout << Grid[i] << endl;
}

struct coord{
	int x;
	int y;
};

coord FIND_WHITE_SPACE_COORDINATES(const int arr[][3]){
	coord c;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (arr[i][j] == 0){
				c.x = j;
				c.y = i;
			}
		}
	}
	return c;
}

void Copy_Grid(int arr[][3], const int obj[][3]){

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++)
			arr[i][j] = obj[i][j];
	}

}

struct Node{
	int Grid[3][3];
	Node *Parent;
	Node *Up;
	Node *Down;
	Node *Left;
	Node *Right;
	int g;
	int h;
	int f;
	coord White_Space_Coordinates;
};

Node* NEW_NODE(const int arr[][3]){

	Total_States++;

	Node *newNode = new Node;
	Copy_Grid(newNode->Grid, arr);
	newNode->White_Space_Coordinates = FIND_WHITE_SPACE_COORDINATES(arr);
	newNode->Parent = nullptr;
	newNode->Up = nullptr;
	newNode->Down = nullptr;
	newNode->Left = nullptr;
	newNode->Right = nullptr;
	newNode->h = HEURISTIC(arr);

	return newNode;
}

void POPULATE_CHILDREN(Node *node){

	int temp[3][3];

	if (node->White_Space_Coordinates.y != 0){		//UP
		Copy_Grid(temp, node->Grid);
		swap(temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x], temp[node->White_Space_Coordinates.y - 1][node->White_Space_Coordinates.x]);
		node->Up = NEW_NODE(temp);
		node->Up->Parent = node;
		node->Up->g = node->g + 1;
		node->Up->f = node->Up->g + node->Up->h;
	}

	if (node->White_Space_Coordinates.y != 2){		//DOWN
		Copy_Grid(temp, node->Grid);
		swap(temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x], temp[node->White_Space_Coordinates.y + 1][node->White_Space_Coordinates.x]);
		node->Down = NEW_NODE(temp);
		node->Down->Parent = node;
		node->Down->g = node->g + 1;
		node->Down->f = node->Down->g + node->Down->h;
	}

	if (node->White_Space_Coordinates.x != 0){		//LEFT
		Copy_Grid(temp, node->Grid);
		swap(temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x], temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x - 1]);
		node->Left = NEW_NODE(temp);
		node->Left->Parent = node;
		node->Left->g = node->g + 1;
		node->Left->f = node->Left->g + node->Left->h;
	}

	if (node->White_Space_Coordinates.x != 2){		//RIGHT
		Copy_Grid(temp, node->Grid);
		swap(temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x], temp[node->White_Space_Coordinates.y][node->White_Space_Coordinates.x + 1]);
		node->Right = NEW_NODE(temp);
		node->Right->Parent = node;
		node->Right->g = node->g + 1;
		node->Right->f = node->Right->g + node->Right->h;
	}
}

bool GOAL_TEST(Node *node){

	States_Explored++;
	if (node->h == 0)
		return true;
	return false;
}

bool isEqual(const int arr1[][3], const int arr2[][3]){
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (arr1[i][j] != arr2[i][j])
				return false;
		}
	}
	return true;
}

int A_STAR_RESULTS(Node *node, double time){

	vector<Node*> path;
	Node *curr = node;
	while (curr != nullptr){
		path.push_back(curr);
		curr = curr->Parent;
	}

	ofstream fout("A_Star Result.txt");
	fout << "\t* Time: " << time << " seconds\n";
	fout << "\t* No. of Steps: " << path.size() - 1 << endl;
	fout << "\t* No. of States explored: " << States_Explored << endl;
	fout << "\t* Total No. of States: " << Total_States << endl;
	fout << "\t* Maximum Memory taken by States: " << (double)(sizeof(Node)*Total_States) / 1024 << " KB's" << endl << endl;

	for (int i = path.size() - 1; i >= 0; i--)
		OUTPUT(path.at(i)->Grid, fout);

	cout << "\n\t* Results of A* search have been placed in the file\n\n";
	return path.size() - 1;

}

void DFS_RESULTS(Node *node, double time){

	vector<Node*> path;
	Node *curr = node;
	while (curr != nullptr){
		path.push_back(curr);
		curr = curr->Parent;
	}

	ofstream fout("DFS Result.txt");
	fout << "\t* Time: " << time << " seconds\n";
	fout << "\t* No. of Steps: " << path.size() - 1 << endl;
	fout << "\t* No. of States explored: " << States_Explored << endl;
	fout << "\t* Maximum Memory taken by States: " << (double)(sizeof(Node)*Total_States) / (1024 * 1024) << " MB's" << endl << endl;

	for (int i = path.size() - 1; i >= 0; i--)
		OUTPUT(path.at(i)->Grid, fout);

}

int MIN_ELEMENT_INDEX(vector<Node*> list){
	int index = 0;

	for (int i = 1; i < list.size(); i++){
		if (list.at(i)->f < list.at(index)->f)
			index = i;
	}
	return index;
}

Node* DFS_REC(Node *node){

	if (node != nullptr){

		Node *result = nullptr;
		if (GOAL_TEST(node))
			result = node;
		else{

			result = DFS_REC(node->Up);
			if (result == nullptr)
				result = DFS_REC(node->Down);
			if (result == nullptr)
				result = DFS_REC(node->Left);
			if (result == nullptr)
				result = DFS_REC(node->Right);
		}

		return result;
	}
	else
		return nullptr;
}

void DFS(Node *root){

	States_Explored = 0;
	Node *result;
	if (root != nullptr){
		clock_t tStart = clock();
		if (GOAL_TEST(root))
			result = root;
		else{
			result = DFS_REC(root->Up);
			if (result == nullptr)
				result = DFS_REC(root->Down);
			if (result == nullptr)
				result = DFS_REC(root->Left);
			if (result == nullptr)
				result = DFS_REC(root->Right);
		}
		if (result != nullptr){
			double time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
			DFS_RESULTS(result, time);
			cout << "\n\t* Results of DFS have been placed in the file\n\n";
		}
		else
			cout << "Result not found\n\n";

	}
	else
		cout << "\tTree doesn't exist for DFS\n\n";
}

int A_STAR(Node *root){

	int index;
	vector<Node*> list;
	Node *curr, *found = nullptr;

	list.push_back(root);

	clock_t tStart = clock();
	while (list.size() != 0){

		index = MIN_ELEMENT_INDEX(list);

		curr = list.at(index);

		if (GOAL_TEST(curr)){

			list.erase(list.begin() + index);
			if (found == nullptr || (found != nullptr && curr->f < found->f)){
				found = curr;

				for (int i = 0; i < list.size(); i++){
					if (list.at(i)->f >= found->f)
						list.erase(list.begin() + i);
				}
			}
		}
		else{
			list.erase(list.begin() + index);
			if (found == nullptr || (found != nullptr && curr->f < found->f)){
				POPULATE_CHILDREN(curr);

				if (curr->Up != nullptr){
					if (curr->Parent == nullptr)		//for ROOT
						list.push_back(curr->Up);
					else if (curr->Parent != nullptr && !isEqual(curr->Up->Grid, curr->Parent->Grid))
						list.push_back(curr->Up);
				}

				if (curr->Down != nullptr){
					if (curr->Parent == nullptr)
						list.push_back(curr->Down);
					else if (curr->Parent != nullptr && !isEqual(curr->Down->Grid, curr->Parent->Grid))
						list.push_back(curr->Down);
				}

				if (curr->Left != nullptr){
					if (curr->Parent == nullptr)
						list.push_back(curr->Left);
					else if (curr->Parent != nullptr && !isEqual(curr->Left->Grid, curr->Parent->Grid))
						list.push_back(curr->Left);
				}

				if (curr->Right != nullptr){
					if (curr->Parent == nullptr)
						list.push_back(curr->Right);
					else if (curr->Parent != nullptr && !isEqual(curr->Right->Grid, curr->Parent->Grid))
						list.push_back(curr->Right);
				}
			}
		}
	}

	double time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	return A_STAR_RESULTS(found, time);


}

int NO_OF_INVERSIONS(int Grid[][3]){

	int temp[9], inversions = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++)
			temp[3 * i + j] = Grid[i][j];
	}

	for (int i = 0; i < 9; i++){
		if (temp[i] != 0){
			for (int j = i + 1; j < 9; j++){
				if (temp[j] != 0 && temp[j] > temp[i])
					inversions++;
			}
		}
	}
	return inversions;
}

void DELETE_TREE(Node *root){

	if (root != nullptr){
		Node *node;
		vector<Node*> del;
		del.push_back(root);

		while (del.size() != 0){
			node = del.at(0);

			if (node->Up != nullptr)
				del.push_back(node->Up);
			if (node->Down != nullptr)
				del.push_back(node->Down);
			if (node->Left != nullptr)
				del.push_back(node->Left);
			if (node->Right != nullptr)
				del.push_back(node->Right);

			del.erase(del.begin());
			delete node;
		}
	}
}

int main(){

	int Grid[3][3], height;

	_INPUT(Grid);

	if (NO_OF_INVERSIONS(Grid) % 2){
		cout << "\n--> The given input is unsolvable for the GOAL STATE having a decreasing series of 8 to 1 numbered tiles\n\n";
		system("pause");
		return 0;
	}


	Node *root = NEW_NODE(Grid);
	root->g = 0;

	height = A_STAR(root);
	
	DFS(root);

	system("pause");
	return 0;
}