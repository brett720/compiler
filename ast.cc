/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include <string>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <map>
using namespace std;

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
}

SymbolTable *Node::symtab = new SymbolTable();
int Node::varNotInitCounter = 0;
int Node::registerCounter = 1; // start with 1 for convenience assigning name for registers
int Node::labelCounter = 0;
vector<TACObject> Node::TACContainer = {};

/* The Print method is used to print the parse tree nodes.
 * If this node has a location (most nodes do, but some do not), it
 * will first print the line number to help you match the parse tree 
 * back to the source text. It then indents the proper number of levels 
 * and prints the "print name" of the node. It then will invoke the
 * virtual function PrintChildren which is expected to print the
 * internals of the node (itself & children) as appropriate.
 */
void Node::Print(int indentLevel, const char *label) { 
    const int numSpaces = 3;
    printf("\n");
    if (GetLocation()) 
        printf("%*d", numSpaces, GetLocation()->first_line);
    else 
        printf("%*s", numSpaces, "");
    printf("%*s%s%s: ", indentLevel*numSpaces, "", 
           label? label : "", GetPrintNameForNode());
   PrintChildren(indentLevel);
} 
	 
string Node::Emit() {
    cout << "In Node class's Emit()" << endl;
    return NULL;
}     
Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

void Identifier::PrintChildren(int indentLevel) {
    printf("%s", name);
}

// ==========================================
// 
bool is_number(const std::string& s)
{
    return( strspn( s.c_str(), "-.0123456789" ) == s.size() );
}

vector<string> createVarList(vector<TACObject> currTac){
    vector<string> varList = {};
    for (int i = 0; i < currTac.size(); i++){
        if(currTac[i].type == 2){
           
            string varName = currTac[i].localVarName;
            if(std::find(varList.begin(), varList.end(),varName)==varList.end()){
                varList.push_back(varName);
                //cout << "here:" << varName << endl;
            }
        }
    }

    return varList;
}
// 3 options:  1: no optimization for 3 const params. 
//             2: Deal with 3 or more const params, but changed all t1 := ~~~, x := t1  to  x := ~~~
//             3: trace back to detect is there a temp var has been folded before. 
//             
//             Using 3 right now.
vector<TACObject> constantFolding(vector<TACObject> tac)
{   
    //return tac;
    vector<TACObject> improvedTac;
    
    for (int i = 0; i < tac.size(); i++) {
        if(tac[i].type == 12 ){
            TACObject newTacObj;
            if(is_number(tac[i].lhs) && is_number(tac[i].rhs)){
                int l = std::stoi(tac[i].lhs);
                int r = std::stoi(tac[i].rhs);
                int result = 0;
                newTacObj.type = 2;
                

                if(tac[i].ops == "+"){
                    result = l + r;  
                }
                else if(tac[i].ops == "-"){
                    result = l - r;
                }
                else if(tac[i].ops == "*"){
                    result = l * r;
                }
                else if(tac[i].ops == "/"){
                    result = l / r;
                }


                newTacObj.localAssignedValue = std::to_string(result);
                newTacObj.id = tac[i + 1].id;
                newTacObj.countedAllocation = tac[i + 1].countedAllocation;
                newTacObj.getFolded = 1;

                if(tac[i+1].type == 2)
                {   
                    newTacObj.localVarName = tac[i+1].localVarName;
                    i++;
                }
                else if(tac[i + 1].type == 12){
                    newTacObj.localVarName = tac[i].toCurVar;
                }
                
                

                improvedTac.push_back(newTacObj);
            }
            else{
                improvedTac.push_back(tac[i]);
            }
        }
        else{
            improvedTac.push_back(tac[i]);
        }
    }

    // option 2:
    // eliminate  t1 = 2 + y;  x = t1;  => x = 2 + y
    //cout << " --in position--" << improvedTac.size()<< endl;
    // for (int i = 0; i < improvedTac.size() - 1; i++) {
    //     if(improvedTac[i].type == 12 && improvedTac[i + 1].type == 2 && improvedTac[i].toCurVar == improvedTac[i + 1].localAssignedValue )
    //     {   
    //         improvedTac[i].toCurVar = improvedTac[i + 1].localVarName;
    //         improvedTac[i + 1].type  = 0;
    //     }

    // }
    

    // option 3.
    for (int i = 0; i < improvedTac.size() - 1; i++) {
        if(improvedTac[i].type == 12 && improvedTac[i + 1].type == 2 && improvedTac[i].toCurVar == improvedTac[i + 1].localAssignedValue )
        {   
            bool detect = true;
            int index = i;
            while(index >= 1 &&  detect){
                if(improvedTac[index].lhs != improvedTac[index - 1].localVarName && improvedTac[index].rhs != improvedTac[index - 1].localVarName){
                    detect = false;
                }
                else{
                    if(improvedTac[index - 1].getFolded == 0){
                        detect = false;
                    }
                    else{
                        break;
                    }

                }

                index--;
            }

            if(detect == true){
                improvedTac[i].toCurVar = improvedTac[i + 1].localVarName;
                improvedTac[i + 1].type  = 0;
            }
        }

    }
    return improvedTac;
}


// My option: disable optimization when x = 1; x = 2;  or  t1 = a(value) + b(value); x = t1. <-- after this line
vector<TACObject> constantPropogation(vector<TACObject> tac)
{   
    vector<TACObject> improvedTac;
    bool needOpt = true;
    bool ifElseBlockUsed = false;
    bool inAIfElseBlock = false;

    std::map<std::string, std::string> varMap;


    // using a loop to check out which assign I should keep in type
    for (int i = 0; i < tac.size(); i++){
        TACObject newTacObj;

        // solve the issue for one extra assignment in if else block
        if(tac[i].type == 10){
            inAIfElseBlock = true;
            ifElseBlockUsed = false;
        }
        if(tac[i].endOfIf == 1){
            inAIfElseBlock = false;
            ifElseBlockUsed = false;
        }


        if(tac[i].type == 12 ){
            if(varMap.find(tac[i].lhs) != varMap.end() && needOpt){

                //cout <<" :" << tac[i].toCurVar << endl;
                if(varMap.find(tac[i].toCurVar) != varMap.end()){
                    if(inAIfElseBlock && ifElseBlockUsed == false){
                        ifElseBlockUsed = true;
                    }
                    else{
                        needOpt = false;
                    }
                }
                else{
                    newTacObj.id = tac[i].id;
                    newTacObj.lhs = varMap[tac[i].lhs];
                    newTacObj.rhs = tac[i].rhs;
                    newTacObj.ops = tac[i].ops;
                    newTacObj.type = 12;
                    newTacObj.toCurVar = tac[i].toCurVar;
                    newTacObj.countedAllocation = tac[i].countedAllocation; 
                }
                
            }
            if(varMap.find(tac[i].rhs) != varMap.end() && needOpt ){
                
                
                if(varMap.find(tac[i].toCurVar) != varMap.end()){
                    if(inAIfElseBlock && ifElseBlockUsed == false){
                        ifElseBlockUsed = true;
                    }
                    else{
                        needOpt = false;
                    }
                }

                // if y = a + b.
                else if(newTacObj.type == 12){
                    newTacObj.rhs = varMap[tac[i].rhs];
                    
                }
                else{
                    newTacObj.id = tac[i].id;
                    newTacObj.lhs = tac[i].lhs;
                    newTacObj.rhs = varMap[tac[i].rhs];
                    newTacObj.ops = tac[i].ops;
                    newTacObj.type = 12;
                    newTacObj.toCurVar = tac[i].toCurVar;
                    newTacObj.countedAllocation = tac[i].countedAllocation; 
                    
                }


            }


        }

        if(tac[i].type == 2 ){

            if(varMap.find(tac[i].localVarName) != varMap.end()){
                if(inAIfElseBlock && ifElseBlockUsed == false){
                        ifElseBlockUsed = true;
                    }
                    else{
                        needOpt = false;
                    }
            }

            if(is_number(tac[i].localAssignedValue)){
                varMap[tac[i].localVarName] = tac[i].localAssignedValue;
            }

            if(varMap.find(tac[i].localAssignedValue) != varMap.end() && needOpt){

                
                newTacObj.id = tac[i].id;
                newTacObj.type = 2;
                newTacObj.localAssignedValue = varMap[tac[i].localAssignedValue];
                newTacObj.localVarName = tac[i].localVarName;
                newTacObj.countedAllocation = tac[i].countedAllocation; 
                
            }
        }


        if(newTacObj.type == 0){
            improvedTac.push_back(tac[i]);
        }
        else{
            improvedTac.push_back(newTacObj);
        }

    }

    return improvedTac;
}


struct usedList
{
    int used[99] = {};
    int size = 0;
};

// if code not used in if else and after if-esle. remove it.
vector<TACObject> deadCodeElimination(vector<TACObject> tac)
{
    vector<TACObject> improvedTac;
    // key to store var, 
    std::map<std::string, usedList> usedMap;
    std::map<std::string, usedList> unusedMap;

    bool inLoop = false; 

    for (int i = 0; i < tac.size(); i++){

        if(tac[i].type == 10){
            usedList newUnused;
            newUnused.used[newUnused.size] = i;
            newUnused.size += 1;
            usedMap[tac[i].conditionVar] = newUnused;
        }
        if(tac[i].type == 12){
            if(unusedMap.find(tac[i].toCurVar) == unusedMap.end()){
                usedList newUnused;
                newUnused.used[newUnused.size] = i;
                newUnused.size += 1;

                unusedMap[tac[i].toCurVar] = newUnused;
            }
            else{
                usedList existUnused = unusedMap[tac[i].toCurVar];
                existUnused.used[existUnused.size] = i;
                existUnused.size += 1;

                unusedMap[tac[i].toCurVar] = existUnused;
            }
            

            if(is_number(tac[i].lhs) == false){

                if(usedMap.find(tac[i].lhs) == usedMap.end()){
                    usedList newUsedAssign;
                    newUsedAssign.used[newUsedAssign.size] = i;
                    newUsedAssign.size += 1;
                    
                    usedMap[tac[i].lhs] = newUsedAssign;
                }
                else{
                    usedList existUnsed = usedMap[tac[i].lhs];
                    existUnsed.used[existUnsed.size] = i;
                    existUnsed.size += 1;
                    usedMap[tac[i].lhs] = existUnsed;
                }


                // usedList newLeftUsed;
                // newLeftUsed.used[newLeftUsed.size] = i;
                // newLeftUsed.size += 1;

                // usedMap[tac[i].lhs] = newLeftUsed;
            }

            if(is_number(tac[i].rhs) == false){
                if(usedMap.find(tac[i].rhs) == usedMap.end()){
                    usedList newUsedAssign;
                    newUsedAssign.used[newUsedAssign.size] = i;
                    newUsedAssign.size += 1;
                    
                    usedMap[tac[i].rhs] = newUsedAssign;
                }
                else{
                    usedList existUnsed = usedMap[tac[i].rhs];
                    existUnsed.used[existUnsed.size] = i;
                    existUnsed.size += 1;
                    usedMap[tac[i].rhs] = existUnsed;
                }


                // usedList newRightUsed;
                // newRightUsed.used[newRightUsed.size] = i;
                // newRightUsed.size += 1;
                
                // usedMap[tac[i].rhs] = newRightUsed;
            }
            
        }

        if(tac[i].type == 2){

            if(unusedMap.find(tac[i].localVarName) == unusedMap.end() ){
                usedList newUnused1;
                newUnused1.used[newUnused1.size] = i;
                newUnused1.size += 1;
                unusedMap[tac[i].localVarName] = newUnused1;
            }
            else{
                usedList existUnunsed = unusedMap[tac[i].localVarName];
                existUnunsed.used[existUnunsed.size] = i;
                existUnunsed.size += 1;
                unusedMap[tac[i].localVarName] = existUnunsed;
            }
            

            

            if(is_number(tac[i].localAssignedValue) == false){

                if(usedMap.find(tac[i].localAssignedValue) == usedMap.end()){
                    usedList newUsedAssign;
                    newUsedAssign.used[newUsedAssign.size] = i;
                    newUsedAssign.size += 1;
                    
                    usedMap[tac[i].localAssignedValue] = newUsedAssign;
                }
                else{
                    usedList existUnsed = usedMap[tac[i].localAssignedValue];
                    existUnsed.used[existUnsed.size] = i;
                    existUnsed.size += 1;
                    usedMap[tac[i].localAssignedValue] = existUnsed;
                }

            }
        }

    }

    // for(auto elem : unusedMap)
    // {
    //    std::cout << elem.first << " " << elem.second.used[0] << endl;
    // }
    

    //type = 2:  if currVar < nearest used && no other currVar in next few lines before the nearest used
    
    for (int i = 0; i < tac.size(); i++){
        //TACObject newTacObj;


        if(tac[i].type == 13 ){
            string labelName = tac[i].label;

            for(int j = i; j < tac.size(); j++){
                if(tac[j].type == 11 && tac[j].targetLocation == labelName && tac[j].endOfForLoop == 1){
                    inLoop = true;
                }
            }

            if(tac[i].endOfForLoop == 1){
                inLoop = false;
            }
        }

        if(tac[i].type == 2){

            if(usedMap.find(tac[i].localVarName) == usedMap.end()){
                tac[i].deadMarked = 1;
            }
            else{
                usedList findUsed = usedMap[tac[i].localVarName];
                int nearestUsed = -1;
                // find nearest used.
                for(int j = 0; j < findUsed.size; j++){
                    if(i < findUsed.used[j]){
                        nearestUsed = findUsed.used[j];
                        break;
                    }
                }
                
                if(nearestUsed == -1){
                    tac[i].deadMarked = 1;
                }
                // find next assign to same variable between i and nearetUsed
                
                for (int j = i + 1; j < nearestUsed; j++){
                    if(tac[i].localVarName == tac[j].localVarName){
                        tac[i].deadMarked = 1;
                        break;
                    }
                }
            }
            
        }


        if(tac[i].type == 12){
            if(usedMap.find(tac[i].toCurVar) == usedMap.end()){
                tac[i].deadMarked = 1;
            }
            else{
                usedList findUsed = usedMap[tac[i].toCurVar];
                int nearestUsed = -1;
                // find nearest used.
                for(int j = 0; j < findUsed.size; j++){
                    if(i < findUsed.used[j]){
                        nearestUsed = findUsed.used[j];
                        break;
                    }
                }
                
                if(nearestUsed == -1){
                    tac[i].deadMarked = 1;
                }
                // find next assign to same variable between i and nearetUsed
                
                for (int j = i + 1; j < nearestUsed; j++){
                    if(tac[i].toCurVar == tac[j].toCurVar){
                        tac[i].deadMarked = 1;
                        break;
                    }
                }
            }
        }

        if(tac[i].type == 2){
            string assignVar = tac[i].localAssignedValue;
            string subs = assignVar.substr(1, assignVar.length());
            char firstChar = char(assignVar.at(0));
            if(firstChar == 't' && is_number(subs)){
                tac[i].deadMarked = 0;
            }
        }

        // if(tac[i].type ==12){
        //     string assignVar = tac[i].toCurVar;
        //     string subs = assignVar.substr(1, assignVar.length());
        //     char firstChar = char(assignVar.at(0));
        //     if(firstChar == 't' && is_number(subs)){
        //         tac[i].deadMarked = 0;
        //     }
        // }



        if(tac[i].deadMarked == 0 || inLoop == true){
            improvedTac.push_back(tac[i]);
        }
    }


    return improvedTac;
}

