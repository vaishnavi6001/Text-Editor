// TEXT EDITOR

// Team Members:
// Dakka Vaishnavi - 2020A7PS0049H
// Abhinav Jagan Polimera - 2020A7PS2192H
// Sanjuktha Anem - 2020A7PS0001H

// Project description: This is a very basic implementation of a text editor. We store the lines of the file in a linked list
// and perform the required operations.
// A word of caution: If two lines are being written in the same input space then they will be treated as a part of one line(node)
// only. Any changes done to this node will affect the full text in it.

// Data Structues used: Linked Lists, Stack

//----------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <stack>
#include <algorithm>
#include <string>

using namespace std;

class Node // The Node class which we'll be using to traverse our linked list
{
public:
    string data;
    Node *next;
};

class Command // The class used to store the necessary variables for our undo and redo stack
{
public:
    int commandNum, lineNum;
    string text, newText;

    Command(int cmdNum, int lineNo, string txt)
    {
        commandNum = cmdNum;
        lineNum = lineNo;
        text = txt;
    }
};

class linkedList // The class used to store all the lines of our file and execute the functions
{
private:
    Node *head, *tail;
    int totalLines;
    string fName;
    stack<Command> undoStack;
    stack<Command> redoStack;

public:
    linkedList();
    void openFile();
    void saveFile();
    void undo();
    void redo();
    void deleteLine(int lineNo, int u);
    void replaceLine(string line, int lineNo, int u);
    void insertAtEnd(string data, int lineNo, int u);
    void insertLineInFile(string line, int lineNo, int u);
    void findWord(string str);
    void printAllLines();
    void wordcount();
    // helper functions-
    void addtotail(string line);
};

linkedList::linkedList() // constructor of our linked list
{
    head = NULL;
    tail = NULL;
    totalLines = 0;

    openFile(); // opening a file before entering the while loop to keep a file ready for further steps

    while (true)
    { // while loop for running the operations continuously until exited
        int num;
        cout << "Commands available: \n1.Open a file\n2.Insert a line at a specified line number\n3.Insert a line at the end\n"
                "4.Delete a line\n5.Replace text in a line\n6.Find a word\n7.Undo\n8.Redo\n9.Print all lines in file\n10.Save changes\n11.Word count\n12.Exit\n";
        cout << "Enter the number of the command you would like to execute: ";
        cin >> num;
        if (num == 1)
        { // to open a file
            openFile();
        }
        else if (num == 2)
        { // to insert a line between the file
            cout << "Enter line number to be inserted at: ";
            int a;
            cin >> a;
            cout << "Enter line to be inserted: ";
            string s;
            cin.ignore();
            getline(cin, s);
            if (a <= totalLines && a > 0)
            {
                insertLineInFile(s, a, 0);
            }
            else
            {
                cout << "Line number not in range. Inserting line at end." << endl;
                insertAtEnd(s, totalLines + 1, 0);
            }
        }
        else if (num == 3)
        { // to insert a line at the end of the file
            string s;
            cout << "Enter line to be inserted at the end: ";
            cin.ignore();
            getline(cin, s);
            insertAtEnd(s, totalLines + 1, 0);
        }
        else if (num == 4)
        { // to delete a line
            int a;
            cout << "Enter line number to be deleted: ";
            cin >> a;
            if (a > totalLines || a <= 0)
            {
                cout << "Number not in range." << endl;
            }
            else
            {
                deleteLine(a, 0);
            }
        }
        else if (num == 5)
        { // to replace a line
            string s;
            int n;
            cout << "Enter line number to be replaced: ";
            cin >> n;
            cout << "Enter new line: ";
            cin.ignore();
            getline(cin, s);
            replaceLine(s, n, 0);
        }
        else if (num == 6)
        { // to find a word in the file
            string s;
            cout << "Enter word to find: ";
            cin >> s;
            findWord(s);
        }
        else if (num == 7)
        { // undo last command
            undo();
        }
        else if (num == 8)
        { // redo
            redo();
        }
        else if (num == 9)
        { // printing all the lines in the file
            printAllLines();
        }
        else if (num == 10)
        { // save all changes in the file
            saveFile();
        }
        else if (num == 11) // tells the number of words in file
        {
            wordcount();
        }
        else if (num == 12) // exit editor
        {
            cout << "Thank you for using our text editor." << endl;
            break;
        }
        else // error message
        {
            cout << "Enter a valid number." << endl;
        }
    }
}

void linkedList::openFile()
{
    cout << "Enter filename: "; // filename is asked
    cin >> fName;
    fstream file;
    string line;
    file.open(fName, ios::in);  // file is opened. If a file of the name entered does not exist then a new blank file is created
    while (getline(file, line)) // parsing the contents of the file into a linkedlist
    {
        addtotail(line);
    }
    file.close(); // closing the file
}

void linkedList::saveFile() // function to save the updated linked list into our file
{
    fstream file;
    file.open(fName, ios::trunc | ios::out); // file is opened
    Node *temp = head;                       // using a temporary pointer to point to the head of the linked list
    while (temp != NULL)                     // looping through and saving all the lines in the file
    {
        file << temp->data << endl; // endl used to store each node as a separate line
        temp = temp->next;
    }
    file.close(); // closing the file
}

// command numbers:
// 1-insert
// 2-delete
// 3-replace text

//the parameter 'u' taken in every function is to ensure that any function executed to perform an undo or redo does not again get 
//stored in the undo stack as it is not an actual command which was initiated by the user, but was just used to undo or redo the
//previous command told by the user.

void linkedList::undo()                                         // function to undo our last command
{
    if (undoStack.empty())
    {                                                           // if there was no command executed previously, then there is nothing to undo.
        cout << "No command is there to undo." << endl;
        return;                                                 // so function will return back
    }
    // ELSE
    Command temp = undoStack.top();                             // top of the undo stack is stored in a temporary pointer
    redoStack.push(temp);                                       // this temp is pushed to the redo stack as the person might want to redo this command later
    undoStack.pop();                                            // the top of the stack is popped as the undo will be performed now

    if (temp.commandNum == 1)                                   // this means the last command was an insert function which now needs to be deleted
    {
        cout << "Removing line inserted previously." << endl;
        deleteLine(temp.lineNum, 1);                            // delete
    }       
    else if (temp.commandNum == 2)                              // this means that the last command was a delete function so the line needs to be inserted once again
    {
        cout << "Inserting line previously deleted." << endl;
        insertLineInFile(temp.text, temp.lineNum, 1);           // insert
    }
    else if (temp.commandNum == 3)                              //this means that a line was replaced in the last command, so the overridden line needs to be brought back
    {
        cout << "Replacing the line with the previous line." << endl;
        replaceLine(temp.text, temp.lineNum, 1);                // replacing with the overridden line
    }
}

void linkedList::redo()                                         // function to redo our last undo
{
    if (redoStack.empty())
    {                                                           // if the redo stack is empty, no command was previously undone
        cout << "No command is there to redo." << endl;
        return;                                                 // so function will return back
    }
    // ELSE
    Command temp = redoStack.top();                             // top of the redo stack is taken in a temporary pointer
    undoStack.push(temp);                                       // this command is also pushed back to the undo stack as one might want to undo it again
    redoStack.pop();                                            // this command will be popped out of redo stack as the redo will be now performed

    if (temp.commandNum == 1)                                   // this means that a line was deleted in the undo step so it needs to be inserted back.
    {
        cout << "Inserting line previously deleted." << endl;
        insertLineInFile(temp.text, temp.lineNum, 0);           // insert
    }
    else if (temp.commandNum == 2)                              // this means that a line was inserted in the undo step so it needs to be deleted
    {
        cout << "Deleting line previously inserted." << endl;
        deleteLine(temp.lineNum, 0);                            // delete
    }
    else if (temp.commandNum == 3)                              // this means that a line was replaced in the undo step. The previous text will be brought back which was there before undo.
    {
        cout << "Replacing the line with the previous line." << endl;
        replaceLine(temp.newText, temp.lineNum, 0);             // replace
    }
}

void linkedList::deleteLine(int lineNo, int u)
{
    string line;
    if (head == NULL) // if linkedlist is empty, it prints empty
    {
        cout << "Empty" << endl;
        return;
    }
    else if (head == tail) // if there's only one element present in linkedlist
    {
        Node *temp = head;
        line = head->data;
        delete (temp);
        head = NULL;        //declare head and tail as NULL
        tail = NULL;
        totalLines--;
    }
    else if (lineNo == 1)
    {

        Node *temp = head;
        Node *nextNode = head->next;
        head = nextNode; // allocating the second node as first node of head and deleting first line
        line = temp->data;
        delete (temp);  
        totalLines--;
    }
    else if (lineNo == totalLines)
    {
        Node *temp = head;
        while (temp->next->next != NULL)
        {
            temp = temp->next; // allocating temp node as second node from last
        }
        tail = temp;         //  which means assigning temp as last node
        line = temp->next->data;
        delete (temp->next);  //deleting last element
        temp->next = NULL;   //making its next element as null
        totalLines--;
    }
    else if (lineNo < totalLines)
    {

        Node *temp = head;
        for (int i = 2; i < lineNo; i++)  //using  for loop for traversing to the given lineNo
        {
            if (temp->next != NULL)
            {
                temp = temp->next; // allocating temp node to the given lineNo
            }
        }
        line = temp->next->data;
        temp->next = temp->next->next; //skipping the given lineNo in between and assigning its next element as its next->next element
        totalLines--;
    }
    if (u != 1)
    {
        Command *nw = new Command(2, lineNo, line);
        undoStack.push(*nw);
    }
}

void linkedList::replaceLine(string line, int lineNo, int u)
{
    if (totalLines < lineNo || lineNo < 1) // if input greater than available lines or line number is less than 1
    {
        cout << "Line doesn't exist" << endl;
    }

    else if (totalLines >= lineNo)
    {
        Node *temp = head;
        int k = 1;
        while (k < lineNo)              //using while loop for traversing through the linkedlist
        {
            temp = temp->next;          // allocating temp node to the given lineNo
            k++;
        }
        string prevLine = temp->data;
        temp->data = line;              // assigning the given data to temp node data
        if (u != 1)
        {
            Command *nw = new Command(3, lineNo, prevLine);
            nw->newText = line;
            undoStack.push(*nw);
        }
    }
}

void linkedList::insertAtEnd(string line, int lineNo, int u)
{
    addtotail(line); // using addtotail function which is used to add element to the end of the linked list
    if (u != 1)
    {
        Command *nw = new Command(1, totalLines, line);
        undoStack.push(*nw);
    }
}

void linkedList::insertLineInFile(string line, int lineNo, int u)
{
    if (lineNo == 1)
    {
        Node *new_Node = new Node(); // creating a new node
        new_Node->data = line;       // storing the line in the new node
        new_Node->next = head;       // assigning the value of head to the node which is after the newly created node
        head = new_Node;             // assigning the newly added node as head
        totalLines++;
    }
    else if (lineNo <= totalLines)
    {
        Node *tmp = new Node; // creating a new node
        tmp->data = line;     // storing the line in the new node

        int countLine = 1;
        Node *traverse = head; // initialising traverse node as head
        while (countLine != lineNo - 1)
        {
            traverse = traverse->next; // traversing upto the line number where the line has to be added
            countLine++;
        }
        tmp->next = traverse->next; // assigning the value of node which is in the lineNo+1 node to the node which is after the newly creadted node
        traverse->next = tmp;       // assigning the newly added node as the node after lineNo node
        totalLines++;
    }
    else if(lineNo>totalLines){
        insertAtEnd(line, totalLines+1, u);
    }
    if (u != 1)
    {
        Command *nw = new Command(1, lineNo, line);
        undoStack.push(*nw);
    }
}

void linkedList::findWord(string str)
{
    Node *current = head; // initialising current node as head
    int count = 1;
    while (current != NULL)
    {                                      // traversing through all the lines stored in the linked list
        int val = current->data.find(str); // using the .find() function to find if the word we are searching is present in the line
        if (val == -1)
        { // if the string which is to be found is not present .find() returns -1 which is stored in val and then count increases by 1
            count++;
        }
        else
        {
            cout << "The word is on line " << count << endl; // prints the line number where the string which is to be found is present
            return;
        }
        current = current->next;
    }
    cout << "Word not found!" << endl;
}

void linkedList::printAllLines()
{                      // function used to print all the lines in our linked list
    Node *temp = head; // a temporary pointer is used to traverse through our linkedlist
    while (temp != NULL)
    {
        cout << temp->data << endl;
        temp = temp->next;
    }
}

void linkedList::addtotail(string line) // helper function used to add a line at the end of our linked list
{
    if (head == NULL) // if the linked list is empty
    {
        Node *tmp;
        tmp = new Node;
        tmp->data = line; // a new Node is created which contains the line
        tmp->next = NULL;
        head = tmp;  // this node becomes the head of our linked list
        tail = head; // head is also the tail as there is only 1 node
        totalLines = 1;
    }
    else // if the linked list is not empty
    {
        Node *tmp;
        tmp = new Node; // a new node is created to store our new line
        tmp->data = line;
        tmp->next = NULL;
        tail->next = tmp; // this new node is taken as the next element of our tail
        tail = tmp;       // this new node becomes the new tail
        totalLines++;
    }
}

void linkedList::wordcount()
{
    string s;
    int l;
    int p = 1;
    int res = 0;

    if (head == NULL)
    {
        cout << "Empty" << endl;                    // if linkedlist is empty
        return;
    }
    else
    {
        Node *temp = head;
        while (temp != NULL)
        {
            s = temp->data;
            l = s.length();

            if (l != 0)
            {
                for (int i = 0; i < l; i++)      //used to check for any spaces, full stops, exclamations or question marks.
                {
                    if (s[i] == ' ' || (s[i] == '.' && s[i+1]!='\0') || (s[i] == '!' && s[i+1]!='\0' && s[i+1]!=' ') || (s[i] == '?' && s[i+1]!='\0' && s[i+1]!=' '))
                    {
                        p++;
                    }
                }
            }
            else if (l == 0)
            {
                p = 0;
            }
            res += p;
            temp = temp->next;
            p = 1;
        }
    }
    cout << "No.of words:";

    cout << res << endl;
}


int main()
{
    linkedList a;
    return 0;
}