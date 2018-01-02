/**
 * File: ast.h
 * ----------- 
 * This file defines the abstract base class Node and the concrete 
 * Identifier and Error node subclasses that are used through the tree as 
 * leaf nodes. A parse tree is a hierarchical collection of ast nodes (or, 
 * more correctly, of instances of concrete subclassses such as VarDecl,
 * ForStmt, and AssignExpr).
 * 
 * Location: Each node maintains its lexical location (line and columns in 
 * file), that location can be NULL for those nodes that don't care/use 
 * locations. The location is typcially set by the node constructor.  The 
 * location is used to provide the context when reporting semantic errors.
 *
 * Parent: Each node has a pointer to its parent. For a Program node, the 
 * parent is NULL, for all other nodes it is the pointer to the node one level
 * up in the parse tree.  The parent is not set in the constructor (during a 
 * bottom-up parse we don't know the parent at the time of construction) but 
 * instead we wait until assigning the children into the parent node and then 
 * set up links in both directions. The parent link is typically not used 
 * during parsing, but is more important in later phases.
 *
 * Printing: The only interesting behavior of the node classes for pp2 is the 
 * bility to print the tree using an in-order walk.  Each node class is 
 * responsible for printing itself/children by overriding the virtual 
 * PrintChildren() and GetPrintNameForNode() methods. All the classes we 
 * provide already implement these methods, so your job is to construct the
 * nodes and wire them up during parsing. Once that's done, printing is a snap!

 */

#ifndef _H_ast
#define _H_ast

#include <stdlib.h>   // for NULL
#include <iostream>
#include <string>
#include "location.h"
#include <vector>
#include <set>

using namespace std;
class SymbolTable;


static vector<string> vList; 
static vector<string> fList;
static vector<string> functionLabel;
static int functionParamId = 0;
struct TACObject{
    // Design your own TAC data structure for each line of TAC
    // type indicate what type of tac stored
    
        int type;
        int trigger;
        int hasOperator;
        int countedAllocation;
        int getFolded;
        int deadMarked;
        int endOfIf;
        int endOfForLoop;
        int beqResult;
        string id; 
        string lhs;
        string rhs;
        string ops;
        // 12 for combine var
        string toCurVar;
        
        // 1 for global variable
        string globalAssignedValue;
        string globalVarName;

        // 2 for local variable
        string localAssignedValue;
        string localVarName;

        // 3 for function name
        string functionName;

        // 4 for end function
        string endFunction;

        // 5 for function begin with allocation
        string allocatedSize;

        // 6 for pushParam
        string pushedVarName;

        // 7 for popParam
        string poppedVarName;

        // 77 for load param
        string loadVarName; 

        // 8 for function call in body
        string paramNum;
        string callfunctionName;
        string assignToTempVar;

        // 9 for return value
        string returnedString; 

        // 10 for if goto
        string conditionVar;
        string branchVar;

        // 11 for Branch
        string targetLocation;

        // 13 for label
        string label;

        //14 print int
        //
        string printInt;
        

        TACObject(){
            type = 0;
            getFolded = 0;
            trigger = 0;
            hasOperator = 0;
            countedAllocation = 0;
            endOfIf = 0;
            deadMarked = 0;
            endOfForLoop = 0;

            beqResult = 1;
            id = "";
            lhs = "";
            rhs = "";
            ops = "";  

            label = "";
            printInt = "";
            
            toCurVar = "";
            globalAssignedValue = "";
            globalVarName = "";
            localAssignedValue = "";
            localVarName = "";

            functionName = "";
            endFunction = "";

            allocatedSize = "";
            pushedVarName = "";
            poppedVarName = "";
            loadVarName = "";

            paramNum = "";
            callfunctionName = "";
            assignToTempVar = "";
            returnedString = "";
            conditionVar = "";
            branchVar = "";
            targetLocation = "";

        }


        void printByLines(){

            switch(type){
                case 14:    cout << "    Print " << printInt << endl;
                            break;
                case 13:    cout << label << ":" << endl;
                            break;
                case 12:    cout << "    " << toCurVar << " := " << lhs 
                                << " " << ops << " " << rhs << endl;
                            break;

                case 1:     cout << "    " << globalVarName << " := " 
                                 << globalAssignedValue << endl;
                            break;
                case 2:     cout << "    " << localVarName << " := " 
                                 << localAssignedValue << endl;
                            break; 

                case 3:     cout << functionName << ":" << endl;
                            break;

                case 4:     cout << "    EndFunc " << endl;
                            break;

                case 5:     cout << "    BeginFunc " << allocatedSize << endl;
                            break;

                case 6:     cout << "    PushParam " << pushedVarName << endl;
                            break;

                case 77:    cout << "    LoadParam " << loadVarName << endl;
                            break;

                case 7 :    cout << "    PopParam " << poppedVarName << endl;
                            break;

                case 8:     cout << "    " << assignToTempVar << " call " 
                                 << callfunctionName << " " << paramNum << endl;
                            break;

                case 9 :    cout << "    Return " <<  returnedString << endl;
                            break;

                case 10:    cout << "    if " << conditionVar << " goto " << branchVar << endl;
                            break;

                case 11:    cout << "    goto " << targetLocation << endl;
                            break; 

            }
        }


        int getOffset(string var){
            for(int i = 0; i < vList.size(); i++){
                if(var == vList[i]){
                    return (i + 1) * 4;
                }
            }

            return -1;
        }

        int getFOffset(string var){
            for(int i = 0; i < fList.size(); i++){
                if(var == fList[i]){
                    return -1 * (i + 1) * 4;
                }
            }

            return -1;
        }

        bool checkIsNumber(const std::string& s)
        {
            return( strspn( s.c_str(), "-.0123456789" ) == s.size() );
        }
        void printMips(){
            int varOffset;
            int localVarOffset;
            int l;
            int r;
            int functionOffset;
            switch(type){
                case 14:    //cout << "    Print " << printInt << endl;
                            //need to find var offset.
                            varOffset = getOffset(printInt);
                            cout << "    " << "lw $t0," << varOffset<<"($sp)" << endl;
                            cout << "    " << "li $v0, 1" << endl;  
                            cout << "    move $a0, $t0" << endl; 
                            cout << "    " << "syscall" << endl;
                            break;
                case 13:    cout << label << ":" << endl;
                            break;
                case 12:    //cout << "    " << toCurVar << " := " << lhs 
                            //    << " " << ops << " " << rhs << endl;
                            
                            if(checkIsNumber(lhs)){
                                cout << "    li $t1," << lhs << endl;
                            }
                            else{
                                l = getOffset(lhs);
                                if(l == -1){
                                    l = getFOffset(lhs);
                                }

                                if( l == -1){
                                    cout << "    move $t1, $t0" << endl;
                                }
                                else{
                                    cout << "    lw $t1, " << l << "($sp)" << endl;
                                }

                            }

                            if(checkIsNumber(rhs)){
                                cout << "    li $t2," << rhs << endl;
                            }
                            else{
                                r = getOffset(rhs);
                                if( r == -1){
                                    r = getFOffset(rhs);
                                }

                                if( r == -1){
                                    cout << "   move $t2, $t0" << endl;
                                }
                                cout << "    lw $t2, " << r << "($sp)" << endl;

                            }

                            if(ops == "<"){
                                cout << "    slt $s0, $t1, $t2" << endl; 
                            }
                            else if(ops == "<="){
                                cout << "    slt $s0, $t2, $t1" << endl; 
                            
                            }
                            else if(ops == "+"){
                                cout << "    add $t0, $t1, $t2" << endl;
                            }
                            

                            break;

                case 1:     cout << "    " << globalVarName << " := " 
                                 << globalAssignedValue << endl;
                            break;
                case 2:     //cout << "    " << localVarName << " := " 
                            //     << localAssignedValue << endl;
                            localVarOffset = getOffset(localVarName);
                            if(localVarOffset == -1){
                                localVarOffset = getFOffset(localVarName);
                            }
                            if(checkIsNumber(localAssignedValue)){

                                cout << "    " << "li $t0, " << localAssignedValue << endl;
                                cout << "    " << "sw $t0, " << localVarOffset << "($sp)" << endl;
                            }
                            else{
                                // add a check for assigned var is not temp var
                                
                                //cout << "    " << "sw $" << localAssignedValue <<", " << localVarOffset << "($sp)" << endl;
                                cout << "    " << "sw $t0, " << localVarOffset << "($sp)" << endl;
                            } 
                            break; 

                case 3:     cout << functionName << ":" << endl;
                            break;

                case 4:     //cout << "    EndFunc " << endl;
                            cout << "    li $v0, 10" << endl;
                            cout << "    syscall" << endl;
                            break;

                case 5:     break;

                case 6:     //cout << "    PushParam " << pushedVarName << endl;
                            localVarOffset = getOffset(pushedVarName);
                            functionParamId += 1;
                            functionOffset = - 1 * (functionParamId * 4);
                            cout << "    " << "lw $t0, " << localVarOffset << "($sp)" << endl;
                            cout << "    " << "sw $t0, " << functionOffset << "($sp)" << endl;

                            break;

                case 77:    //cout << "    LoadParam " << loadVarName << endl;
                            fList.push_back(loadVarName);
                            break;

                case 7 :    //cout << "    PopParam " << poppedVarName << endl;
                            //functionParamId = functionParamId - int(poppedVarName) / 4;
                            cout << "    move $t0, $v0" << endl;
                            break;

                case 8:     //cout << "    " << assignToTempVar << " call " 
                            //     << callfunctionName << " " << paramNum << endl;
                            
                            if(callfunctionName == "readIntFromSTDIN"){
                                cout << "    " << "li $v0, 5" << endl;  
                                cout << "    " << "syscall" << endl;
                                //cout << "    " << "move $ << assignToTempVar << ", $v0" << endl;
                                cout << "    " << "move $t0, $v0" << endl;
                            
                            }
                            else{
                                cout << "    jal " << callfunctionName << endl;
                                cout << "f1:" << endl;
                            }

                            break;

                case 9 :    //cout << "    Return " <<  returnedString << endl;

                            //check if it in function:
                            //localVarOffset = getOffset(returnedString);
                            cout << "    move $v0, $t0" << endl;
                            cout << "    jal f1" << endl;
                            break;

                case 10:    //cout << "    if " << conditionVar << " goto " << branchVar << endl;

                            cout << "    beq $s0, " << beqResult << ", " << branchVar << endl;
                            break;

                case 11:    //cout << "    goto " << targetLocation << endl;
                            cout << "    j " <<  targetLocation << endl;
                            break; 

            }
        }


};


vector<TACObject> constantFolding(vector<TACObject> tac);
vector<TACObject> constantPropogation(vector<TACObject> tac);
vector<TACObject> deadCodeElimination(vector<TACObject> tac);

bool is_number(const std::string& s);
vector<string> createVarList(vector<TACObject> currTac);
class Node  {
  protected:
    yyltype *location;
    Node *parent;
    static SymbolTable *symtab;

    // Declare any global variables you need here
    // And initialize them in ast.cc
    static int registerCounter;
    static int varCounter;
    static int labelCounter;
    static int varNotInitCounter;
    static int checkFlag;
    static vector<TACObject> TACContainer;
    //static vector<string> garbage;
  public:
    Node(yyltype loc);
    Node();
    virtual ~Node() {}
    
    yyltype *GetLocation()   { return location; }
    void SetParent(Node *p)  { parent = p; }
    Node *GetParent()        { return parent; }

    virtual const char *GetPrintNameForNode() = 0;
    
    // Print() is deliberately _not_ virtual
    // subclasses should override PrintChildren() instead
    void Print(int indentLevel, const char *label = NULL); 
    virtual void PrintChildren(int indentLevel)  {}
    virtual string Emit();
};
   

class Identifier : public Node 
{
  protected:
    char *name;
    
  public:
    Identifier(yyltype loc, const char *name);
    const char *GetPrintNameForNode()   { return "Identifier"; }
    void PrintChildren(int indentLevel);
    char *GetName() const { return name; }

    // virtual string Emit();
};


// This node class is designed to represent a portion of the tree that 
// encountered syntax errors during parsing. The partial completed tree
// is discarded along with the states being popped, and an instance of
// the Error class can stand in as the placeholder in the parse tree
// when your parser can continue after an error.
class Error : public Node
{
  public:
    Error() : Node() {}
    const char *GetPrintNameForNode()   { return "Error"; }
};



#endif
