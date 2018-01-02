/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"
#include "string"
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this);
}


VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}

void VarDecl::PrintChildren(int indentLevel) {
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
   if (assignTo) assignTo->Print(indentLevel+1, "(initializer) ");
}
//TODO
string VarDecl::Emit() {
    
    string varName = GetIdentifier()->GetName();
    TACObject newTac;
    
    if (assignTo) {
        string rhsRegisterName = assignTo->Emit();
        newTac.id = varName;
        newTac.type = 2;
        newTac.countedAllocation = 1;
        newTac.localAssignedValue = rhsRegisterName;
        newTac.localVarName = varName;
        //newTac.line = string("    ") + varName + string(" := ") + rhsRegisterName;

        // if global var, type = 3.
        // 
        //int typeResult = 1;
        // for(int i = 0; i < TACContainer.size(); i++){
        //     std::string eachline = TACContainer[i].line;
    
        //     //cout << "----WWWarning: " <<  TACContainer[i].line << endl;
        //     if(eachline.at(0)  != ' '){
        //         typeResult = 3;
        //         //cout << "WWWarning: " <<  newTac.line << endl;
        //         break;
        //     }
        // }
        // if var in function, type = 1.
        //newTac.type = typeResult;
        TACContainer.push_back(newTac);    
    }
    

    

    return "VarDecl::Emit()";
}



FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) {
    (body=b)->SetParent(this);
}


//TODO not complete count part
//
string FnDecl::Emit() {
    string varName = id->GetName();
    TACObject funcName;
    //funcName.id = " ";
    //funcName.line = varName + ":";
    funcName.type = 3;
    funcName.trigger = 1;
    funcName.functionName = varName;
    TACContainer.push_back(funcName);

    int count = 0;
    for(int i = 0; i < formals->NumElements(); i++) {
        formals->Nth(i)->Emit();
        VarDecl* v = formals->Nth(i);
        string sname = v->GetIdentifier()->GetName();

        TACObject newTac ;
        newTac.id = sname;
        newTac.type = 77;
        newTac.loadVarName = sname;
        newTac.countedAllocation = 1;
        //newTac.line = string("    ") + "LoadParam" + string(" ") + sname;
        //newTac.type = 1;
        count++;
        TACContainer.push_back(newTac); 
    }
    int bcount = 0;
    if(body){
        body->Emit();

        for(int i = 0; i < TACContainer.size(); i++){
            if(TACContainer[i].countedAllocation == 1){
                bcount++;
                //cout << "warning:" << TACContainer[i].id <<endl;
                TACContainer[i].countedAllocation = 0;
            }
        }


        bcount = bcount * 4;
        TACObject test ;
        
        int loc = 0;
        for (int i=TACContainer.size() - 1; i >= 0; i--){
            if(TACContainer[i].type == 3 && TACContainer[i].trigger == 1){
                loc = i;
                TACContainer[i].trigger = 0;
            }
        }

        test.type = 5;
        test.allocatedSize = std::to_string(bcount);
        //test.line = string("    BeginFunc ")+  std::to_string(bcount);
        //test.type = 0;
        
        TACContainer.insert(TACContainer.begin()+loc+count + 1, test); 

    }
    TACObject endFunc ;
    endFunc.type = 4;
    //endFunc.id = " ";
    //
    // endFunc.line = "    EndFunc ";
    // endFunc.type = 0;
    TACContainer.push_back(endFunc);
    return "FnDecl::Emit()";
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}


