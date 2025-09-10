#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <sstream>
#include <set>
#include <random>
#include <fstream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

// SarcasmLang Grammar:
// program    := line*
// line       := INSULT ':' statement
// statement  := assignment | ifstmt | whilestmt | printstmt
// assignment := IDENTIFIER '=' expression
// ifstmt     := 'obviously' expression 'then' '{' line* '}'
// whilestmt  := 'whatever' expression 'do' '{' line* '}'
// printstmt  := ('show' | 'display' | 'reveal' | 'output') expression
// expression := term (('plus' | 'minus' | '+' | '-') term)*
// term       := factor (('times' | 'divided_by' | '*' | '/') factor)*
// factor     := NUMBER | IDENTIFIER | '(' expression ')'

enum TokenType {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_ASSIGN,
    TOKEN_COLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_OBVIOUSLY,
    TOKEN_WHATEVER,
    TOKEN_THEN,
    TOKEN_DO,
    TOKEN_SHOW,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_INSULT,
    TOKEN_WORD_PLUS,
    TOKEN_WORD_MINUS,
    TOKEN_WORD_MULTIPLY,
    TOKEN_WORD_DIVIDE
};

// Comprehensive list of creative insults for SarcasmLang
std::set<std::string> insults = {
    "idiot", "moron", "dummy", "fool", "genius", "einstein", "smartass", "brainiac",
    "doofus", "numbskull", "dimwit", "nincompoop", "bonehead", "knucklehead",
    "airhead", "birdbrain", "blockhead", "chucklehead", "fathead", "meathead",
    "pinhead", "hotshot", "wiseguy", "smarty", "clever_clogs", "know_it_all",
    "rocket_scientist", "mastermind", "prodigy", "savant", "intellectual",
    "scholar", "philosopher", "thinker", "genius_level", "big_brain",
    "smooth_brain", "pea_brain", "walnut_brain", "goldfish_brain",
    "caveman", "neanderthal", "primitive", "amateur", "rookie", "newbie",
    "peasant", "pleb", "scrub", "noob", "casual", "try_hard", "wannabe"
};

struct Token {
    TokenType type;
    std::string value;
    double numValue;
};

class SarcasmLexer {
private:
    std::string input;
    size_t pos;
    
    std::string readWord() {
        std::string word;
        while (pos < input.length() && 
               (std::isalnum(static_cast<unsigned char>(input[pos])) || input[pos] == '_')) {
            word += static_cast<char>(std::tolower(static_cast<unsigned char>(input[pos++])));
        }
        return word;
    }
    
public:
    SarcasmLexer(const std::string& text) : input(text), pos(0) {}
    
    Token nextToken() {
        while (pos < input.length() && std::isspace(static_cast<unsigned char>(input[pos]))) {
            pos++;
        }
        
        if (pos >= input.length()) {
            return {TOKEN_EOF, "", 0};
        }
        
        char current = input[pos];
        
        if (std::isdigit(static_cast<unsigned char>(current))) {
            std::string number;
            while (pos < input.length() && 
                   (std::isdigit(static_cast<unsigned char>(input[pos])) || input[pos] == '.')) {
                number += input[pos++];
            }
            return {TOKEN_NUMBER, number, std::stod(number)};
        }
        
        if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
            std::string word = readWord();
            
            // Check for keywords
            if (word == "obviously") return {TOKEN_OBVIOUSLY, word, 0};
            if (word == "whatever") return {TOKEN_WHATEVER, word, 0};
            if (word == "then") return {TOKEN_THEN, word, 0};
            if (word == "do") return {TOKEN_DO, word, 0};
            if (word == "show" || word == "display" || word == "reveal" || word == "output") {
                return {TOKEN_SHOW, word, 0};
            }
            if (word == "plus") return {TOKEN_WORD_PLUS, word, 0};
            if (word == "minus") return {TOKEN_WORD_MINUS, word, 0};
            if (word == "times") return {TOKEN_WORD_MULTIPLY, word, 0};
            if (word == "divided_by") return {TOKEN_WORD_DIVIDE, word, 0};
            
            // Check if it's an insult
            if (insults.find(word) != insults.end()) {
                return {TOKEN_INSULT, word, 0};
            }
            
            return {TOKEN_IDENTIFIER, word, 0};
        }
        
        pos++;
        switch (current) {
            case '=': return {TOKEN_ASSIGN, "=", 0};
            case ':': return {TOKEN_COLON, ":", 0};
            case '(': return {TOKEN_LPAREN, "(", 0};
            case ')': return {TOKEN_RPAREN, ")", 0};
            case '{': return {TOKEN_LBRACE, "{", 0};
            case '}': return {TOKEN_RBRACE, "}", 0};
            case '+': return {TOKEN_PLUS, "+", 0};
            case '-': return {TOKEN_MINUS, "-", 0};
            case '*': return {TOKEN_MULTIPLY, "*", 0};
            case '/': return {TOKEN_DIVIDE, "/", 0};
            case '<': return {TOKEN_LESS, "<", 0};
            case '>': return {TOKEN_GREATER, ">", 0};
            default: return {TOKEN_EOF, "", 0};
        }
    }
};

// Abstract Syntax Tree nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual Value* codegen() = 0;
};

class NumberExprAST : public ASTNode {
    double val;
public:
    NumberExprAST(double val) : val(val) {}
    Value* codegen() override;
};

class VariableExprAST : public ASTNode {
    std::string name;
public:
    VariableExprAST(const std::string& name) : name(name) {}
    Value* codegen() override;
};

class BinaryExprAST : public ASTNode {
    char op;
    std::unique_ptr<ASTNode> lhs, rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    Value* codegen() override;
};

class AssignmentAST : public ASTNode {
    std::string varName;
    std::unique_ptr<ASTNode> expr;
public:
    AssignmentAST(const std::string& varName, std::unique_ptr<ASTNode> expr)
        : varName(varName), expr(std::move(expr)) {}
    Value* codegen() override;
};

class PrintAST : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::string printWord;
public:
    PrintAST(std::unique_ptr<ASTNode> expr, const std::string& printWord) 
        : expr(std::move(expr)), printWord(printWord) {}
    Value* codegen() override;
};

class IfAST : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenStmts;
public:
    IfAST(std::unique_ptr<ASTNode> condition, std::vector<std::unique_ptr<ASTNode>> thenStmts)
        : condition(std::move(condition)), thenStmts(std::move(thenStmts)) {}
    Value* codegen() override;
};

class WhileAST : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
public:
    WhileAST(std::unique_ptr<ASTNode> condition, std::vector<std::unique_ptr<ASTNode>> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    Value* codegen() override;
};

class SarcasmLineAST : public ASTNode {
    std::string insult;
    std::unique_ptr<ASTNode> statement;
public:
    SarcasmLineAST(const std::string& insult, std::unique_ptr<ASTNode> statement)
        : insult(insult), statement(std::move(statement)) {}
    Value* codegen() override;
};

// Global LLVM objects
static LLVMContext theContext;
static IRBuilder<> builder(theContext);
static std::unique_ptr<Module> theModule;
static std::map<std::string, AllocaInst*> namedValues;

// Helper function to create entry block alloca
static AllocaInst* createEntryBlockAlloca(Function* function, const std::string& varName) {
    IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(Type::getDoubleTy(theContext), nullptr, varName);
}

// Random insult generator for runtime fun
std::string generateRandomInsult() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::vector<std::string> insultList(insults.begin(), insults.end());
    std::uniform_int_distribution<> dis(0, static_cast<int>(insultList.size()) - 1);
    return insultList[dis(gen)];
}

// Code generation implementations
Value* NumberExprAST::codegen() {
    return ConstantFP::get(theContext, APFloat(val));
}

Value* VariableExprAST::codegen() {
    AllocaInst* alloca = namedValues[name];
    if (!alloca) {
        Function* function = builder.GetInsertBlock()->getParent();
        alloca = createEntryBlockAlloca(function, name);
        namedValues[name] = alloca;
        builder.CreateStore(ConstantFP::get(theContext, APFloat(0.0)), alloca);
    }
    return builder.CreateLoad(Type::getDoubleTy(theContext), alloca, name);
}

Value* BinaryExprAST::codegen() {
    Value* l = lhs->codegen();
    Value* r = rhs->codegen();
    if (!l || !r) return nullptr;
    
    switch (op) {
        case '+': return builder.CreateFAdd(l, r, "addtmp");
        case '-': return builder.CreateFSub(l, r, "subtmp");
        case '*': return builder.CreateFMul(l, r, "multmp");
        case '/': return builder.CreateFDiv(l, r, "divtmp");
        case '<': return builder.CreateUIToFP(builder.CreateFCmpULT(l, r, "cmptmp"),
                                            Type::getDoubleTy(theContext), "booltmp");
        case '>': return builder.CreateUIToFP(builder.CreateFCmpUGT(l, r, "cmptmp"),
                                            Type::getDoubleTy(theContext), "booltmp");
        default: return nullptr;
    }
}

Value* AssignmentAST::codegen() {
    Value* val = expr->codegen();
    if (!val) return nullptr;
    
    AllocaInst* alloca = namedValues[varName];
    if (!alloca) {
        Function* function = builder.GetInsertBlock()->getParent();
        alloca = createEntryBlockAlloca(function, varName);
        namedValues[varName] = alloca;
    }
    
    builder.CreateStore(val, alloca);
    return val;
}

Value* PrintAST::codegen() {
    Value* val = expr->codegen();
    if (!val) return nullptr;
    
    // Create printf function if it doesn't exist
    Function* printfFunc = theModule->getFunction("printf");
    if (!printfFunc) {
        std::vector<Type*> printfArgs;
        printfArgs.push_back(PointerType::get(Type::getInt8Ty(theContext), 0));

        FunctionType* printfType = FunctionType::get(
            IntegerType::getInt32Ty(theContext),
            printfArgs,
            /*isVarArg=*/true);
        printfFunc = Function::Create(printfType, Function::ExternalLinkage, "printf", theModule.get());
    }
    
    // Create sarcastic format string based on print word
    std::string format;
    if (printWord == "show") format = "Fine, here's your precious number: %.2f\n";
    else if (printWord == "display") format = "Displaying for the visually impaired: %.2f\n";
    else if (printWord == "reveal") format = "The shocking revelation is: %.2f\n";
    else format = "Output (because you demanded it): %.2f\n";
    
    Value* formatStr = builder.CreateGlobalStringPtr(format);
    
    return builder.CreateCall(printfFunc, {formatStr, val});
}

Value* IfAST::codegen() {
    Value* condVal = condition->codegen();
    if (!condVal) return nullptr;
    
    condVal = builder.CreateFCmpONE(condVal, ConstantFP::get(theContext, APFloat(0.0)), "obviouslycond");
    
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* thenBB = BasicBlock::Create(theContext, "obviously_then", function);
    BasicBlock* mergeBB = BasicBlock::Create(theContext, "obviously_cont", function);
    
    builder.CreateCondBr(condVal, thenBB, mergeBB);
    
    builder.SetInsertPoint(thenBB);
    Value* thenVal = nullptr;
    for (auto& stmt : thenStmts) {
        thenVal = stmt->codegen();
    }
    builder.CreateBr(mergeBB);
    
    builder.SetInsertPoint(mergeBB);
    
    return Constant::getNullValue(Type::getDoubleTy(theContext));
}

Value* WhileAST::codegen() {
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* loopBB = BasicBlock::Create(theContext, "whatever_loop", function);
    BasicBlock* bodyBB = BasicBlock::Create(theContext, "whatever_body", function);
    BasicBlock* afterBB = BasicBlock::Create(theContext, "whatever_after", function);
    
    builder.CreateBr(loopBB);
    builder.SetInsertPoint(loopBB);
    
    Value* condVal = condition->codegen();
    if (!condVal) return nullptr;
    
    condVal = builder.CreateFCmpONE(condVal, ConstantFP::get(theContext, APFloat(0.0)), "whatevercond");
    builder.CreateCondBr(condVal, bodyBB, afterBB);
    
    builder.SetInsertPoint(bodyBB);
    for (auto& stmt : body) {
        stmt->codegen();
    }
    builder.CreateBr(loopBB);
    
    builder.SetInsertPoint(afterBB);
    
    return Constant::getNullValue(Type::getDoubleTy(theContext));
}

Value* SarcasmLineAST::codegen() {
    // Add sarcastic comment to LLVM IR
    Value* result = statement->codegen();
    
    // Print the insult as a comment during compilation
    static int lineNum = 1;
    std::cout << "  ; Line " << lineNum++ << ": " << insult 
              << " says something ridiculous" << std::endl;
    
    return result;
}

// SarcasmLang Parser
class SarcasmParser {
private:
    SarcasmLexer lexer;
    Token currentToken;
    
    void nextToken() {
        currentToken = lexer.nextToken();
    }
    
public:
    SarcasmParser(const std::string& input) : lexer(input) {
        nextToken();
    }
    
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseLine();
    std::vector<std::unique_ptr<ASTNode>> parseProgram();
};

std::unique_ptr<ASTNode> SarcasmParser::parseFactor() {
    if (currentToken.type == TOKEN_NUMBER) {
        double val = currentToken.numValue;
        nextToken();
        return std::make_unique<NumberExprAST>(val);
    }
    
    if (currentToken.type == TOKEN_IDENTIFIER) {
        std::string name = currentToken.value;
        nextToken();
        return std::make_unique<VariableExprAST>(name);
    }
    
    if (currentToken.type == TOKEN_LPAREN) {
        nextToken();
        auto expr = parseExpression();
        if (currentToken.type != TOKEN_RPAREN) {
            std::cerr << "genius: Expected ')' but you forgot it, obviously" << std::endl;
            return nullptr;
        }
        nextToken();
        return expr;
    }
    
    return nullptr;
}

std::unique_ptr<ASTNode> SarcasmParser::parseTerm() {
    auto left = parseFactor();
    
    while (currentToken.type == TOKEN_MULTIPLY || currentToken.type == TOKEN_DIVIDE ||
           currentToken.type == TOKEN_WORD_MULTIPLY || currentToken.type == TOKEN_WORD_DIVIDE) {
        char op;
        if (currentToken.type == TOKEN_MULTIPLY || currentToken.type == TOKEN_WORD_MULTIPLY) {
            op = '*';
        } else {
            op = '/';
        }
        nextToken();
        auto right = parseFactor();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> SarcasmParser::parseExpression() {
    auto left = parseTerm();
    
    while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS ||
           currentToken.type == TOKEN_WORD_PLUS || currentToken.type == TOKEN_WORD_MINUS ||
           currentToken.type == TOKEN_LESS || currentToken.type == TOKEN_GREATER) {
        char op;
        if (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_WORD_PLUS) op = '+';
        else if (currentToken.type == TOKEN_MINUS || currentToken.type == TOKEN_WORD_MINUS) op = '-';
        else if (currentToken.type == TOKEN_LESS) op = '<';
        else op = '>';
        
        nextToken();
        auto right = parseTerm();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> SarcasmParser::parseStatement() {
    if (currentToken.type == TOKEN_IDENTIFIER) {
        std::string varName = currentToken.value;
        nextToken();
        if (currentToken.type == TOKEN_ASSIGN) {
            nextToken();
            auto expr = parseExpression();
            return std::make_unique<AssignmentAST>(varName, std::move(expr));
        }
    }
    
    if (currentToken.type == TOKEN_SHOW) {
        std::string printWord = currentToken.value;
        nextToken();
        auto expr = parseExpression();
        return std::make_unique<PrintAST>(std::move(expr), printWord);
    }
    
    if (currentToken.type == TOKEN_OBVIOUSLY) {
        nextToken();
        auto condition = parseExpression();
        if (currentToken.type != TOKEN_THEN) {
            std::cerr << "smartass: Expected 'then' after condition, duh!" << std::endl;
            return nullptr;
        }
        nextToken();
        if (currentToken.type != TOKEN_LBRACE) {
            std::cerr << "blockhead: Expected '{' to start obviously block" << std::endl;
            return nullptr;
        }
        nextToken();
        
        std::vector<std::unique_ptr<ASTNode>> thenStmts;
        while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
            auto line = parseLine();
            if (line) thenStmts.push_back(std::move(line));
        }
        
        if (currentToken.type != TOKEN_RBRACE) {
            std::cerr << "bonehead: Expected '}' to end obviously block" << std::endl;
            return nullptr;
        }
        nextToken();
        
        return std::make_unique<IfAST>(std::move(condition), std::move(thenStmts));
    }
    
    if (currentToken.type == TOKEN_WHATEVER) {
        nextToken();
        auto condition = parseExpression();
        if (currentToken.type != TOKEN_DO) {
            std::cerr << "dimwit: Expected 'do' after whatever condition" << std::endl;
            return nullptr;
        }
        nextToken();
        if (currentToken.type != TOKEN_LBRACE) {
            std::cerr << "numbskull: Expected '{' to start whatever block" << std::endl;
            return nullptr;
        }
        nextToken();
        
        std::vector<std::unique_ptr<ASTNode>> body;
        while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
            auto line = parseLine();
            if (line) body.push_back(std::move(line));
        }
        
        if (currentToken.type != TOKEN_RBRACE) {
            std::cerr << "meathead: Expected '}' to end whatever block" << std::endl;
            return nullptr;
        }
        nextToken();
        
        return std::make_unique<WhileAST>(std::move(condition), std::move(body));
    }
    
    return nullptr;
}

std::unique_ptr<ASTNode> SarcasmParser::parseLine() {
    if (currentToken.type != TOKEN_INSULT) {
        std::cerr << "amateur: Every line must start with an insult, you casual!" << std::endl;
        return nullptr;
    }
    
    std::string insult = currentToken.value;
    nextToken();
    
    if (currentToken.type != TOKEN_COLON) {
        std::cerr << "rookie: Expected ':' after insult '" << insult << "'" << std::endl;
        return nullptr;
    }
    nextToken();
    
    auto statement = parseStatement();
    if (!statement) {
        std::cerr << "noob: Failed to parse statement after '" << insult << ":'" << std::endl;
        return nullptr;
    }
    
    return std::make_unique<SarcasmLineAST>(insult, std::move(statement));
}

std::vector<std::unique_ptr<ASTNode>> SarcasmParser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode>> lines;
    
    while (currentToken.type != TOKEN_EOF) {
        auto line = parseLine();
        if (line) {
            lines.push_back(std::move(line));
        } else {
            std::cerr << "scrub: Parse error encountered" << std::endl;
            break;
        }
    }
    
    return lines;
}

void compileAndRun(const std::string& source) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    
    theModule = std::make_unique<Module>("SarcasmLang", theContext);
    
    SarcasmParser parser(source);
    auto program = parser.parseProgram();
    
    FunctionType* mainType = FunctionType::get(Type::getInt32Ty(theContext), false);
    Function* mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", theModule.get());
    
    BasicBlock* entryBB = BasicBlock::Create(theContext, "entry", mainFunc);
    builder.SetInsertPoint(entryBB);
    
    std::cout << "\n🎭 SarcasmLang Compilation Comments:" << std::endl;
    for (auto& line : program) {
        line->codegen();
    }
    
    builder.CreateRet(ConstantInt::get(Type::getInt32Ty(theContext), 0));
    
    if (verifyFunction(*mainFunc, &errs())) {
        std::cerr << "smarty: Function verification failed, congratulations!" << std::endl;
        return;
    }
    
    std::cout << "\n📝 Generated LLVM IR:" << std::endl;
    theModule->print(outs(), nullptr);
    
    std::string errStr;
    ExecutionEngine* engine = EngineBuilder(std::move(theModule))
                                .setErrorStr(&errStr)
                                .create();
    
    if (!engine) {
        std::cerr << "genius: Failed to create execution engine: " << errStr << std::endl;
        return;
    }
    
    std::cout << "\n🚀 Executing your 'brilliant' SarcasmLang program:" << std::endl;
    std::vector<GenericValue> args;
    engine->runFunction(mainFunc, args);
    
    std::cout << "\n💀 Execution complete. Hope you're satisfied, " 
              << generateRandomInsult() << "!" << std::endl;
    
    delete engine;
}

// Helper function to read file contents
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "genius: Can't open file '" << filename 
                  << "' - did you forget it exists?" << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to show usage
void showUsage(const std::string& programName) {
    std::cout << "🎭 SarcasmLang Compiler Usage (for the clueless):" << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << programName << " [filename.sarcasm]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  filename.sarcasm  - Your insulting source code file" << std::endl;
    std::cout << "  --help, -h        - Show this help (obviously)" << std::endl;
    std::cout << "  --demo            - Run the built-in demo program" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " hello.sarcasm" << std::endl;
    std::cout << "  " << programName << " --demo" << std::endl;
    std::cout << std::endl;
    std::cout << "File Extensions:" << std::endl;
    std::cout << "  .sarcasm    - Standard SarcasmLang files" << std::endl;
    std::cout << "  .insult     - Alternative extension" << std::endl;
    std::cout << "  .attitude   - For programs with extra sass" << std::endl;
}

// Create some example files for the user
void createExampleFiles() {
    // Hello World example
    std::ofstream hello("hello.sarcasm");
    if (hello.is_open()) {
        hello << "genius: x = 42\n";
        hello << "smartass: show x\n";
        hello << "einstein: display x times 2\n";
        hello.close();
        std::cout << "📝 Created hello.sarcasm" << std::endl;
    }
    
    // Factorial example
    std::ofstream factorial("factorial.sarcasm");
    if (factorial.is_open()) {
        factorial << "numbskull: n = 5\n";
        factorial << "bonehead: factorial = 1\n";
        factorial << "meathead: i = 1\n";
        factorial << "dimwit: whatever i < n plus 1 do {\n";
        factorial << "    birdbrain: factorial = factorial times i\n";
        factorial << "    airhead: i = i plus 1\n";
        factorial << "}\n";
        factorial << "smooth_brain: show factorial\n";
        factorial.close();
        std::cout << "📝 Created factorial.sarcasm" << std::endl;
    }
    
    // Loop and conditional example
    std::ofstream complex("complex.sarcasm");
    if (complex.is_open()) {
        complex << "amateur: sum = 0\n";
        complex << "noob: i = 1\n";
        complex << "casual: whatever i < 6 do {\n";
        complex << "    scrub: sum = sum plus i\n";
        complex << "    try_hard: i = i plus 1\n";
        complex << "}\n";
        complex << "wannabe: show sum\n";
        complex << "peasant: obviously sum > 10 then {\n";
        complex << "    pleb: reveal 999\n";
        complex << "    rookie: display sum divided_by 2\n";
        complex << "}\n";
        complex.close();
        std::cout << "📝 Created complex.sarcasm" << std::endl;
    }
    
    std::cout << "\n🎉 Example files created! Try:" << std::endl;
    std::cout << "   ./sarcasmlang hello.sarcasm" << std::endl;
    std::cout << "   ./sarcasmlang factorial.sarcasm" << std::endl;
    std::cout << "   ./sarcasmlang complex.sarcasm" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "🎭 Welcome to SarcasmLang - The Most Insulting Programming Language!" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    // Handle command line arguments
    if (argc < 2) {
        std::cout << "smarty: No file specified, so I'll create some examples for you..." << std::endl;
        std::cout << std::endl;
        createExampleFiles();
        std::cout << std::endl;
        showUsage(argv[0]);
        return 1;
    }
    
    std::string arg = argv[1];
    
    // Handle help
    if (arg == "--help" || arg == "-h") {
        showUsage(argv[0]);
        return 0;
    }
    
    // Handle demo mode
    if (arg == "--demo") {
        std::string program = R"(
idiot: x = 10
moron: y = 5 
dummy: result = x plus y times 2
genius: show result

fool: counter = 1
smartass: whatever counter < 4 do {
    brainiac: display counter
    doofus: counter = counter plus 1
}

numbskull: obviously result > 15 then {
    dimwit: reveal 999
    nincompoop: output 42
}

bonehead: final = result divided_by 2
knucklehead: show final
        )";
        
        std::cout << "🎪 Running built-in demo program:" << std::endl;
        std::cout << "📜 Demo source code:" << std::endl;
        std::cout << program << std::endl;
        compileAndRun(program);
        return 0;
    }
    
    // Handle file input
    std::string filename = arg;
    std::cout << "📁 Reading SarcasmLang file: " << filename << std::endl;
    
    std::string program = readFile(filename);
    if (program.empty()) {
        std::cerr << "dummy: File is empty or couldn't be read. What did you expect?" << std::endl;
        return 1;
    }
    
    std::cout << "📜 Source program from " << filename << ":" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << program << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    compileAndRun(program);
    
    return 0;
}