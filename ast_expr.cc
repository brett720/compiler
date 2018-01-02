/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"


IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) {
    printf("%d", value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

void BoolConstant::PrintChildren(int indentLevel) {
    printf("%s", value ? "true" : "false");
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r)
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r)
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL;
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o)
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}

SelectionExpr::SelectionExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void SelectionExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1);
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::PrintChildren(int indentLevel) {

    if (base) base->Print(indentLevel+1);
    if (field) field->Print(indentLevel+1);
    if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

void VarExpr::PrintChildren(int identLevel) {
  id->Print(identLevel + 1);
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
  id = ident;
}
//finished ====
string IntConstant::Emit() {
  return to_string(value);
}
//finished ====
string BoolConstant::Emit() {
  return value ? "true" : "false";
}
//finished ====
string Operator::Emit() {
  return string(tokenString);
}

string Call::Emit() {
  int count = 0;
  //int inPrint = 0;
  //garbage.push_back("11");
  //
  if(strcmp(field->GetName(),"readIntFromSTDIN") == 0 || strcmp(field->GetName(),"printInt") == 0){
    string registerString = "t" + to_string(registerCounter);
    registerCounter++;
    TACObject newTac1;
    
    if(strcmp(field->GetName(),"printInt") == 0 && actuals->NumElements() == 1){
      string assignto = actuals->Nth(0)->Emit();
      
      newTac1.type = 14;
      newTac1.printInt = assignto;
      TACContainer.push_back(newTac1); 

      //newTac1.id = " ";
      //newTac1.line = string("    Print ") + assignto;
      //inPrint = 1;
    
    }
    else{
      newTac1.type = 8;
      newTac1.countedAllocation = 1;
      newTac1.id = registerString;
      newTac1.assignToTempVar = registerString;
      newTac1.callfunctionName = field->GetName();
      newTac1.paramNum = std::to_string(count);
      TACContainer.push_back(newTac1); 
      //newTac1.line =  string("    ") + registerString + string(" call ") + field->GetName() + string(" ") + std::to_string(count);
    }
    
    // if global var, type = 3.
    // 
    // int typeResult = 1;
    // for(int i = 0; i < TACContainer.size(); i++){
    //     std::string eachline = TACContainer[i].line;
    //     if(eachline.at(0)  != ' '){
    //         typeResult = 3;
    //         break;
    //     }
    // }
    // if var in function, type = 1.
    // if(inPrint == 0){
    //   newTac1.type = typeResult;
    // }
    // else{
    //   newTac1.type = 0;
    // }

    
  }
  else{
    for(int i = 0; i < actuals->NumElements(); i++) {
        
        string sname = actuals->Nth(i)->Emit();

        TACObject newTac ;
        newTac.type = 6;
        newTac.pushedVarName = sname;

        //newTac.line = string("    ") + "PushParam" + string(" ") + sname;
        //newTac.type = 0;
        TACContainer.push_back(newTac); 
        count++;
    }
    
    string registerString = "t" + to_string(registerCounter);
    registerCounter++;

    TACObject newTac1;
    newTac1.type = 8;
    newTac1.countedAllocation = 1;
    newTac1.assignToTempVar = registerString;
    newTac1.id = registerString;
    newTac1.callfunctionName = field->GetName();
    newTac1.paramNum = std::to_string(count);

    TACContainer.push_back(newTac1); 
    //newTac1.id = " ";
    //newTac1.line =  string("    ") + registerString + string(" call ") + field->GetName() + string(" ") + std::to_string(count);
    
    // if global var, type = 3.
    // 
    // int typeResult = 1;
    // for(int i = 0; i < TACContainer.size(); i++){
    //     std::string eachline = TACContainer[i].line;
    //     if(eachline.at(0)  != ' '){
    //         typeResult = 3;
    //         break;
    //     }
    // }
    // // if var in function, type = 1.
    // newTac1.type = typeResult;



    
    count = count * 4;

    TACObject newTac2 ;
    newTac2.type = 7;
    newTac2.poppedVarName = std::to_string(count);
    TACContainer.push_back(newTac2); 

    //newTac2.id = " ";
    //newTac2.line =  string("    ") + string("PopParam ") + std::to_string(count);
    //newTac2.type = 0;
    
  }
  //garbage.pop_back();
  //Decl* returnDecl = symtab->FindSymbolInAllScopes(field->GetName());
  return "t" + to_string(registerCounter - 1);;
}


//TODO
string VarExpr::Emit() {
  string localVarName = id->GetName();
  return string(localVarName);
}

string EmptyExpr::Emit() {
  return "EmptyExpr Emit";
}
//TODO
string ArithmeticExpr::Emit() {
  string leftString = left->Emit();
  string rightString = right->Emit();
  string opString = op->Emit();

  string registerString = "t" + to_string(registerCounter);
  registerCounter++;
  
  TACObject newTac;
  newTac.type = 12;
  newTac.lhs = leftString;
  newTac.rhs = rightString;
  newTac.ops = opString;
  newTac.toCurVar = registerString;
  newTac.id = registerString;
  newTac.countedAllocation = 1;
  TACContainer.push_back(newTac);   

  //newTac.id = " ";
  //newTac.line =  string("    ") + registerString + string(" := ") + leftString + string(" ") + opString + string(" ") + rightString;
  //int typeResult = 1;
  // for(int i = 0; i < TACContainer.size(); i++){
  //     std::string eachline = TACContainer[i].line;
  //     if(eachline.at(0)  != ' '){
  //         typeResult = 3;
  //         break;
  //     }
  // }
  // if var in function, type = 1.
  //newTac.type = typeResult;
  

  return registerString;
}

string RelationalExpr::Emit() {
  string leftString = left->Emit();
  string rightString = right->Emit();
  string opString = op->Emit();

  string registerString = "t" + to_string(registerCounter);
  registerCounter++;
  
  // TACObject newTac;
  // newTac.id = " ";
  // newTac.line =  string("    ") + registerString + string(" := ") + leftString + string(" ") + opString + string(" ") + rightString;
  // int typeResult = 1;
  // for(int i = 0; i < TACContainer.size(); i++){
  //     std::string eachline = TACContainer[i].line;
  //     if(eachline.at(0)  != ' '){
  //         typeResult = 3;
  //         break;
  //     }
  // }
  // // if var in function, type = 1.
  // newTac.type = typeResult;

  TACObject newTac ;
  newTac.type = 12;

  newTac.lhs = leftString;
  newTac.rhs = rightString;
  newTac.ops = opString;
  newTac.toCurVar = registerString;
  newTac.id = registerString;
  newTac.countedAllocation = 1;
  TACContainer.push_back(newTac);   

  return registerString;
}

string AssignExpr::Emit() {
  string l,r,o;
  if(left){
    l = left->Emit();
  }
  if(right){
    r = right->Emit();
  }
  if(op){
    o = op->Emit();
  }
  TACObject newTac ;

  int checkVarInited = 0;
  for(int i = 0; i < TACContainer.size(); i++){
    
    if(l == TACContainer[i].id){
      newTac.id = "";
      newTac.type = 2;
      newTac.localVarName = l;
      newTac.localAssignedValue = r;

      //newTac.line =  string("    ") + l + string(" := ") + r;
      //newTac.type = 0;
      checkVarInited = 1;
      break;
    }
  }
  if(checkVarInited == 0){
    newTac.id = l;
    newTac.type = 2;
    newTac.localVarName = l;
    newTac.localAssignedValue = r;
    //newTac. = 1;
    //newTac.line =  string("    ") + l + string(" := ") + r;
    //newTac.type = 0;
  }
  
  TACContainer.push_back(newTac); 

  return l;
}

string LogicalExpr::Emit() {
  string l,r,o;
  if(left){
    l = left->Emit();
  }
  if(right){
    r = right->Emit();
  }
  if(op){
    o = op->Emit();
  }

  
  return l + " " + o + " " + r ; 
}

string EqualityExpr::Emit() {
  string leftString = left->Emit();
  string rightString = right->Emit();
  string opString = op->Emit();

  string registerString = "t" + to_string(registerCounter);
  registerCounter++;
  
  TACObject newTac ;
  newTac.id = registerString;
  newTac.type = 12;
  newTac.countedAllocation = 1;
  newTac.toCurVar = registerString;
  newTac.lhs = leftString;
  newTac.ops = opString;
  newTac.rhs = rightString;

  //newTac.id = " ";
  //newTac.line =  string("    ") + registerString + string(" := ") + leftString + string(" ") + opString + string(" ") + rightString;
  // int typeResult = 1;
  // for(int i = 0; i < TACContainer.size(); i++){
  //     std::string eachline = TACContainer[i].line;
  //     if(eachline.at(0)  != ' '){
  //         typeResult = 3;
  //         break;
  //     }
  // }
  // if var in function, type = 1.
  //newTac.type = typeResult;
  TACContainer.push_back(newTac);   

  return registerString;
  
  //return "EqualityExpr::Emit()"; 
}

string PostfixExpr::Emit() {
  string l,o;
  if(op){
    o = op->Emit();
  }
  if(left){
    l = left->Emit();
  }
  
  TACObject newTac ;
  newTac.type = 12;

  string registerString = "t" + to_string(registerCounter);
  registerCounter++;

  if(o == "++"){
    newTac.toCurVar = registerString;
    newTac.lhs = l;
    newTac.ops = "+";
    newTac.rhs = "1";

    //newTac.line =  string("    ") + l + string(" := ") + l + " + 1";
  }
  else if(o == "--"){
    newTac.toCurVar = registerString;
    newTac.lhs = l;
    newTac.ops = "-";
    newTac.rhs = "1";
    //newTac.line =  string("    ") + l + string(" := ") + l + " - 1";
  }
  TACContainer.push_back(newTac); 

  TACObject newTac1;
  newTac1.type = 2;
  newTac1.localVarName = l;
  newTac1.localAssignedValue = registerString; 
  TACContainer.push_back(newTac1);  
  // int checkVarInited = 0;
  // for(int i = 0; i < TACContainer.size(); i++){
  //   //strcmp( l.compare(TACContainer[i].id) == 0
  //   if(l == TACContainer[i].id){
  //     checkVarInited = 1;
  //     break;
  //   }
  // }
  // if(checkVarInited == 0){
  //   newTac.id = "";
  //   //newTac.type = 0;
  // }


  

  return "PostfixExpr::Emit()";
}
