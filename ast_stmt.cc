/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

StmtBlock::StmtBlock(List<Stmt*> *s) {
    Assert(s != NULL);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    stmts->PrintAll(indentLevel+1);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) {
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this);
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) {
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) {
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) {
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}

DeclStmt::DeclStmt(yyltype loc, Decl *decl) : Stmt(loc) {
    Assert(decl != NULL);
    (varDecl=decl)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    varDecl->Print(indentLevel+1);
}

// TODO 
string Program::Emit() {
    if ( decls->NumElements() > 0 ) {
      for ( int i = 0; i < decls->NumElements(); ++i ) {
        Decl *d = decls->Nth(i);
        d->Emit();
      }
    }
     //vector<TACObject> optimized_TACContainer = constantFolding(TACContainer);
     //vector<TACObject> optimized_TACContainer = constantPropogation(TACContainer);
    //vector<TACObject> optimized_TACContainer = deadCodeElimination(TACContainer);
    for (int i = 0; i < TACContainer.size(); i++) {
        TACContainer[i].printByLines();
        //optimized_TACContainer[i].printByLines();
    }


    // assign beq result (1 or 0) to [if ~ goto branch] tac.
    // to implement the branch correctly
    for (int i = 0; i < TACContainer.size(); i++) {
        if(TACContainer[i].ops == "<="){
            if(TACContainer[i + 1].type == 10){
                TACContainer[i + 1].beqResult = 0;
            }
        }
        
    }
    cout << "=================================" << endl;
    vList = createVarList(TACContainer);
    
    cout << ".data" << endl;
    cout << ".text" << endl;

    // for (int i = 0; i < TACContainer.size(); i++) {
    //     //TACContainer[i].printMips();
    //     cout << TACContainer[i].type << endl;
    // }


    for (int i = 0; i < TACContainer.size(); i++) {
        
        TACContainer[i].printMips();
    }
    // cout << "var list:" << endl;
    
    return "Program::Emit()";
}

//finished ====
string StmtBlock::Emit() {
    for(int i = 0; i < stmts->NumElements(); i++){
        Stmt* ith_statement = stmts->Nth(i);
        ith_statement->Emit();
    }
    
    return "StmtBlock::Emit()";
}

string ForStmt::Emit() { 
    string retInit;
    if(init){
        retInit = init -> Emit();
    }

    string testLabel;
    string bodyLabel;
    string endLabel;

    testLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;

    bodyLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;

    endLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;
    if(test){

        TACObject newTac1;
        newTac1.type = 13;
        newTac1.label = testLabel;
        //newTac1.line = testLabel + ":";
        //newTac1.type = 0;
        //newTac1.id = " ";
        TACContainer.push_back(newTac1); 

        string res = test->Emit();


        TACObject newTac2;
        newTac2.conditionVar = res;
        newTac2.branchVar = bodyLabel;
        newTac2.type = 10;
        newTac2.countedAllocation = 1;  // int a; for(a= 2; ~~~)
        //newTac2.line = string("    if ") + res + " goto " +  bodyLabel;
        //newTac2.type = 1;
        //newTac2.id = " ";
        TACContainer.push_back(newTac2); 

    }

    TACObject gotoEnd ;
    gotoEnd.type = 11;
    gotoEnd.targetLocation = endLabel;

    //gotoEnd.line = string("    goto ") + endLabel;
    //gotoEnd.type = 0;
    //gotoEnd.id = " ";
    TACContainer.push_back(gotoEnd); 

    if(body){
        TACObject newTac1 ;
        newTac1.type = 13;
        newTac1.label = bodyLabel;


        //newTac1.line = bodyLabel + ":";
        //newTac1.type = 0;
        //newTac1.id = " ";
        TACContainer.push_back(newTac1); 
        body->Emit();



        step->Emit();
        TACObject gotoEnd1 ;
        gotoEnd1.type = 11;
        gotoEnd1.targetLocation = testLabel;
        gotoEnd1.endOfForLoop = 1;
        //gotoEnd1.line = string("    goto ") + testLabel;
        //gotoEnd1.type = 0;
        //gotoEnd1.id = " ";
        TACContainer.push_back(gotoEnd1); 
    }


    TACObject newTac ;
    newTac.label = endLabel;
    newTac.type = 13;

    //newTac.id = " ";
    TACContainer.push_back(newTac); 

    return "ForStmt::Emit()";
}

string WhileStmt::Emit() {
    string testLabel;
    string bodyLabel;
    string endLabel;

    testLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;

    bodyLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;

    endLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;

    if(test){

        TACObject newTac1 ;
        newTac1.type = 13;
        newTac1.label = testLabel;
        

        // TACObject newTac1;
        // newTac1.line = testLabel + ":";

        // newTac1.type = 0;

        // newTac1.id = " ";
        TACContainer.push_back(newTac1); 

        string res = test->Emit();

        TACObject newTac2 ;
        newTac2.conditionVar = res;
        newTac2.branchVar = bodyLabel;
        newTac2.type = 10;


        // TACObject newTac2;
        // newTac2.line = string("    if ") + res + " goto " +  bodyLabel;

        // newTac2.type = 0;
        // newTac2.id = " ";
        TACContainer.push_back(newTac2); 


        TACObject gotoEnd ;
        gotoEnd.type = 11;
        gotoEnd.targetLocation = endLabel;

        // TACObject gotoEnd;
        // gotoEnd.line = string("    goto ") + endLabel;
        // gotoEnd.type = 0;
        // gotoEnd.id = " ";
        TACContainer.push_back(gotoEnd);
    }

    if(body){
        // TACObject newTac1;
        // newTac1.line = bodyLabel + ":";

        // newTac1.type = 0;
        // newTac1.id = " ";
        // 
        TACObject newTac1 ;
        newTac1.type = 13;
        newTac1.label = bodyLabel;

        TACContainer.push_back(newTac1); 

        body->Emit();
        // TACObject gotoEnd1;
        // gotoEnd1.line = string("    goto ") + testLabel;
        // gotoEnd1.type = 0;
        // gotoEnd1.id = " ";
        TACObject gotoEnd1 ;
        gotoEnd1.type = 11;
        gotoEnd1.endOfForLoop = 1;
        gotoEnd1.targetLocation = testLabel;

        TACContainer.push_back(gotoEnd1); 
    }

    // TACObject newTac;
    // newTac.line = endLabel + ":";

    // newTac.type = 0;
    // newTac.id = " ";
    TACObject newTac ;
    newTac.label = endLabel;
    newTac.type = 13;
    

    TACContainer.push_back(newTac);  
    return "WhileStmt";
}

string IfStmt::Emit() {
    string ifLabel;    // 0
    string endLabel;   //4   
    string elseLabel;  // 1    after inner-if statement in ifLabel  go to the end of if and then create end label


    ifLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;
    if(elseBody){
        elseLabel = string("L") + std::to_string(labelCounter);
        labelCounter++;
    }

    if(test){
        string received = test->Emit();
        TACObject newTac ;
        newTac.type = 10;
        newTac.conditionVar = received;
        newTac.branchVar = ifLabel;
        //newTac.line = string("    ") + "if " + received + " goto " + ifLabel;

        //newTac.type = 0;
        //newTac.id = " ";
        TACContainer.push_back(newTac); 

        TACObject newTac0 ;
        newTac0.type = 11; 
        if(elseBody){
            // newTac0.line = string("    ") + "goto " + elseLabel;
            // newTac0.type = 0;
            // newTac0.id = " ";
            newTac0.targetLocation = elseLabel;
            TACContainer.push_back(newTac0);
        }
        else{
            string endOfInner = string("L") + std::to_string(labelCounter) ;

            // newTac0.line = string("    ") + "goto " + endOfInner;
            // newTac0.type = 0;
            // newTac0.id = " ";
            newTac0.targetLocation = endOfInner;
            TACContainer.push_back(newTac0);
        }

         
    }

    if(body){
        
        TACObject newTac;
        newTac.type = 13;
        newTac.label = ifLabel;
        
        // newTac.line = ifLabel + ":";
        // newTac.type = 0;
        // newTac.id = " ";
        TACContainer.push_back(newTac); 

        body->Emit();
    }


    // go to end func
    
    endLabel = string("L") + std::to_string(labelCounter);
    labelCounter++;
    TACObject newTac;
    newTac.type = 11;
    newTac.targetLocation = endLabel;

    // newTac.line = string("    ") +  "goto " + endLabel;

    // newTac.type = 0;
    // newTac.id = " ";
    TACContainer.push_back(newTac); 

    if(elseBody){
        TACObject newTac;
        newTac.type = 13;
        newTac.label = elseLabel;

        // newTac.line = elseLabel + ":";

        // newTac.type = 0;
        // newTac.id = " ";
        TACContainer.push_back(newTac); 
        elseBody->Emit();

        TACObject endTac ;
        endTac.type = 11;
        endTac.targetLocation = endLabel;
        //endTac.line = string("    ") +  "goto " + endLabel;
        //endTac.id = " ";
        TACContainer.push_back(endTac); 
       
    }
    TACObject endTac1 ;
    endTac1.type = 13;
    endTac1.label = endLabel;
    endTac1.endOfIf = 1;

    // endTac1.line = endLabel + ":";
    // endTac1.id = " ";
    TACContainer.push_back(endTac1); 
    
    return "ifStmt";
}

string ReturnStmt::Emit() {
    string ret;
    if(expr){
        ret = expr->Emit();
        TACObject newTac ;
        newTac.type = 9;
        newTac.returnedString = ret;
        // newTac.line = string("    ") + "Return" + string(" ") + ret;
        // newTac.type = 4;
        // newTac.id = " ";
        TACContainer.push_back(newTac); 
        
    }
    return "ReturnStmt Emit()";
}

string DeclStmt::Emit() {
    varDecl->Emit();
    return "DeclStmt Emit()";
}

