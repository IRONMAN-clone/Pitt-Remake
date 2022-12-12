// TODO: LIST
// TODO: Support Hex and Binary and Octal Numbers
// TODO: Add Linux Syscalls Support + Windows Syscalls Support
// TODO: Support argc and argv
// TODO: Add C-String and String-functions
// TODO: Add variable support (in other word -> `bind` keyword)
// TODO: Add function support (Verify `main` if possible) + Return Keyword + Function Scope
// TODO: Add Dynamic Memory support (Heap-Allocated Memory) [`alloc` keyword]
// TODO: Add `import` keyword
// TODO: Add Structures
// TODO: Fix PARSER to support -> "Hello \\"

#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include "./token.hpp"
#include <stack>
using namespace std;

#define PITT_EXT "pitt"

void help()
{
    cout << "USAGE: " << endl;
    cout << "pitt (command) <filename> [subcmd]" << endl;
    cout << "COMMAND: " << endl;
    cout << "       build     simulates the file" << endl;
    cout << "       dump      dumps all the tokens" << endl;
    cout << "SUBCMD: " << endl;
    cout << "       timeit    checks the time of execution" << endl;
}

// DONE: Added a padding of P_EOT (refererring to END_OF_TOKEN) to manage extra spaces on words_count()
// while parsing strings
int words_count(const char *filename)
{
    int _count = 0;
    ifstream file{filename};
    string temp{};

    while (file >> temp)
    {
        _count++;
    }

    file.close();
    return _count;
}

void check_file_ext(const char *filename)
{
    size_t j{};
    for (; j < strlen(filename); j++)
    {
        if (filename[j] == '.' && filename[j + 1] == '/')
        {
        }
        else if (filename[j] == '.')
        {
            break;
        }
    }
    char buffer[strlen(filename) - (j + 1)] = {'\0'};
    uint8_t buf_c = 0;
    j++;
    for (; j < strlen(filename); j++)
    {
        buffer[buf_c++] = filename[j];
    }
    buffer[buf_c++] = '\0';
    if (strcmp(buffer, PITT_EXT) != 0)
    {
        printf("pitt: \033[0;31mfatal error:\033[0;37m invalid file format provided `%s`\n\n", filename);
        exit(1);
    }
}

void check_file_val(const char *filename)
{
    ifstream file{filename};

    if (file.is_open())
    {
        file.close();
    }
    else
    {
        printf("forth:\033[0;31m fatal error:\033[0;37m can not open file to read `%s`\n\n", filename);
        exit(1);
    }
}

bool isnum(string &text)
{
    bool state = false;

    if (text.size() > 0)
    {
        size_t _c = {};
        for (size_t i = 0; i < text.size(); i++)
        {
            if (i == 0 && (text[i] == '-' || text[i] == '+') && text.size() > 1)
            {
                _c++;
            }
            else if (text[i] >= '0' && text[i] <= '9')
            {
                _c++;
            }
        }

        if (_c == text.size())
        {
            state = true;
        }
    }

    return state;
}

PittType fetch_type(string &text)
{
    PittType rettype = P_WORD;
    for (auto it : usable_map)
    {
        if (text == it.first)
        {
            rettype = it.second;
            break;
        }
    }
    return rettype;
}

int parse_words_in_text(string &text)
{
    istringstream serve(text);
    string temp = {};
    int count = 0;

    while (serve >> temp)
    {
        count++;
    }
    return count;
}

// FIXME: ADD SUPPORT FOR `\\"`
void parse(const char *filename, Pitt_token *token_list)
{
    ifstream file{filename};
    string tokens{};
    string temp_buffer{};

    int tc_count = -1;
    int col_ctr = 0;
    int line = {};
    string counter_buffer = "";

    while (getline(file, temp_buffer))
    {
        line++;
        int waitfor = 0;
        bool encountered = false;

        istringstream serve(temp_buffer);
        counter_buffer = temp_buffer;

        while (serve >> tokens)
        {
            // We have to change counter_buffer[col_ctr] to some random trash numbers
            // becase .find() will never check for duplicates and will keep getting the
            // duplicates
            if (tokens == "#")
            {
                break;
            }

            col_ctr = static_cast<int>(counter_buffer.find(tokens));
            counter_buffer[col_ctr] = -1;

            if (waitfor > 0 && encountered == true)
            {
                waitfor--;
                if (waitfor == 0)
                    encountered = false;
            }

            if (tokens[0] == '"' && encountered == false)
            {
                encountered = true;
                bool unescaped = false;
                string lexedstr = "";
                int scopeCount = 0;

                size_t begin = static_cast<size_t>(col_ctr);
                size_t end = begin + 1;

                for (size_t dupend = end; dupend < temp_buffer.size(); dupend++)
                {
                    unescaped = true;
                    if (temp_buffer[dupend] == '"' && temp_buffer[dupend - 1] != '\\')
                    {
                        if (temp_buffer[dupend + 1] == ' ' || temp_buffer[dupend + 1] == '\n' || temp_buffer[dupend + 1] == '\0')
                        {
                            unescaped = false;
                            end = dupend;
                            break;
                        }
                    }
                }

                for (size_t p = begin; p <= end; p++)
                {
                    if (temp_buffer[p] == '"' && temp_buffer[p - 1] != '\\')
                    {
                        scopeCount++;
                    }
                }

                string word_finder = "";

                for (size_t k = begin; k <= end; k++)
                    word_finder += temp_buffer[k];

                for (size_t k = begin + 1; k < end; k++)
                    lexedstr += temp_buffer[k];

                waitfor = parse_words_in_text(word_finder);

                if (unescaped == false && scopeCount == 2)
                {
                    tc_count++;
                    token_list[tc_count].token = lexedstr;
                    token_list[tc_count].r = line;
                    token_list[tc_count].c = col_ctr;
                    token_list[tc_count].type = P_STR;
                }
                else
                {
                    tc_count++;
                    token_list[tc_count].token = lexedstr;
                    token_list[tc_count].r = line;
                    token_list[tc_count].c = col_ctr;
                    token_list[tc_count].type = P_STR;
                    token_list[tc_count].unescaped = true;
                }
            }

            else if (waitfor == 0)
            {
                tc_count++;
                token_list[tc_count].token = tokens;
                token_list[tc_count].r = line;
                token_list[tc_count].c = col_ctr;

                if (isnum(tokens))
                {
                    token_list[tc_count].type = P_INT;
                }
                else
                {
                    token_list[tc_count].type = fetch_type(tokens);
                }
            }
        }
    }

    file.close();
}

int EXPECT_THEN(int current, Pitt_token *list, const size_t &size)
{
    int location = -1;
    current += 1;
    for (; current < static_cast<int>(size); current++)
    {
        if (list[current].type == P_IF)
        {
            break;
        }
        else if (list[current].type == P_WHILE)
        {
            break;
        }
        else if (list[current].type == P_END)
        {
            break;
        }
        else if (list[current].type == P_THEN)
        {
            location = current;
            break;
        }
    }
    return location;
}

int EXPECT_END(int current, Pitt_token *list, const size_t &size, stack<PittType> &ptok)
{
    int location = -1;
    current++;

    int delay = 0;
    for (; current < static_cast<int>(size); current++)
    {
        if (list[current].type == P_DEF || list[current].type == P_IF || list[current].type == P_WHILE)
        {
            delay++;
        }
        else if (list[current].type == P_END)
        {
            if (delay == 0)
            {
                location = current;
                break;
            }
            else
            {
                delay--;
            }
        }
        if (ptok.top() == P_IF)
        {
            if (list[current].type == P_ELIF)
            {
                if (delay == 0)
                {
                    location = current;
                    break;
                }
            }
            else if (list[current].type == P_ELSE)
            {
                if (delay == 0)
                {
                    location = current;
                    break;
                }
            }
        }
    }
    return location;
}

int EXPECT_ONLY_BEGIN(int current, Pitt_token *list, const size_t &size)
{
    int location = -1;
    current++;

    int delay = 0;
    for (; current < static_cast<int>(size); current++)
    {
        if (list[current].type == P_DEF)
        {
            delay++;
        }
        else if (list[current].type == P_BEGIN)
        {
            if (delay == 0)
            {
                location = current;
                break;
            }
            else
            {
                delay--;
            }
        }
    }
    return location;
}

int EXPECT_ONLY_END(int current, Pitt_token *list, const size_t &size)
{
    int location = -1;
    current++;

    int delay = 0;
    for (; current < static_cast<int>(size); current++)
    {
        if (list[current].type == P_IF || list[current].type == P_WHILE || list[current].type == P_DEF)
        {
            delay++;
        }
        else if (list[current].type == P_END)
        {
            if (delay == 0)
            {
                location = current;
                break;
            }
            else
            {
                delay--;
            }
        }
    }
    return location;
}

#define MEM_CAP 140000

void simulate_file(const char *filename, Pitt_token *token_list, const int &argc, char **argv)
{
    stack<int> numeric_stack = {};
    const size_t size = words_count(filename);

    // COMMENTS are not even PARSED
    // If/While handling
    int CondEncounter = {};
    stack<PittType> ptoken;
    stack<int> wloc;
    stack<bool> wcon;

    // Memory Handling
    byte memory[MEM_CAP] = {};
    size_t free_ptr = 0;
    short byte16Memory[MEM_CAP] = {};
    size_t free_ptr_16 = 0;
    int byte32Memory[MEM_CAP] = {};
    size_t free_ptr_32 = 0;

    // Definitions (Not Macros | Not Exactly Functions Either)
    struct Tuple<string, int, Pitt_token> definition[size] =
    {
    };
    int tp = 0;
    stack<int> return_stack;
    bool inDef = false;
    stack<bool> end_behaviour;

    // C-String
    my_stack c_string;
    int argv_ptr = 0;

    int j = {};
    while (j < size)
    {
        if (token_list[j].type != P_EOT)
        {
            if (token_list[j].type == P_INT)
            {
                numeric_stack.push(stoi(token_list[j].token));
            }

            if (token_list[j].type == P_STR)
            {
                if (token_list[j].unescaped == true)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unescaped string literal found" << endl;
                    break;
                }

                string &raw = token_list[j].token;
                bool last_esc = false;
                size_t pusher = free_ptr;

                int j = 0;
                while (j < raw.size())
                {
                    if (raw[j] == '\\')
                    {
                        j++;
                        if (raw[j] == 'n')
                        {
                            memory[pusher] = '\n';
                            pusher++;
                        }
                        else if (raw[j] == 'b')
                        {
                            pusher--;
                            memory[pusher] = '\b';
                            pusher++;
                        }
                        else if (raw[j] == 't')
                        {
                            memory[pusher] = '\t';
                            pusher++;
                        }
                        else if (raw[j] == '0')
                        {
                            memory[pusher] = '\0';
                            pusher++;
                        }
                        else if (raw[j] == '"')
                        {
                            memory[pusher] = '"';
                            pusher++;
                        }
                        else if (raw[j] == '\\')
                        {
                            memory[pusher] = '\\';
                            pusher++;
                        }
                        else if (raw[j] == 'r')
                        {
                            memory[pusher] = '\r';
                            pusher++;
                        }
                        else if (raw[j] == 'v')
                        {
                            memory[pusher] = '\v';
                            pusher++;
                        }
                    }
                    else
                    {
                        memory[pusher] = raw[j];
                        pusher++;
                    }
                    j++;
                }

                numeric_stack.push(free_ptr);
                free_ptr = pusher;
                numeric_stack.push(free_ptr);
            }

            if (token_list[j].type == P_PLUS)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b + a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `PLUS` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_MIN)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b - a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `MINUS` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_MUL)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b * a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `MUL` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_DIV)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b / a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `DIV` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BOR)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b | a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `BOR` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BAND)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b & a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `BAND` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BNOT)
            {
                if (numeric_stack.size() > 0)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(!a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `NOT` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BSHL)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b << a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `SHL` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BSHR)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b >> a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `SHR` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_BXOR)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b ^ a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `XOR` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_NE)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b != a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `!=` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_E)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b == a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `=` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_L)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b < a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `<` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_G)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b > a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `>` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_GEQ)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b >= a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `>=` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_LEQ)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b <= a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `<=` operator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_DUP)
            {
                if (numeric_stack.size() > 0)
                {
                    numeric_stack.push(numeric_stack.top());
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `DUP` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_SWAP)
            {
                if (numeric_stack.size() > 1)
                {
                    float a = numeric_stack.top();
                    numeric_stack.pop();
                    float b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(a);
                    numeric_stack.push(b);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `SWAP` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_DROP)
            {
                if (numeric_stack.size() > 0)
                {
                    numeric_stack.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `DROP` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_OVER)
            {
                if (numeric_stack.size() > 1)
                {
                    float a = numeric_stack.top();
                    numeric_stack.pop();
                    float b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b);
                    numeric_stack.push(a);
                    numeric_stack.push(b);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `OVER` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_ROT)
            {
                if (numeric_stack.size() > 2)
                {
                    float a = numeric_stack.top();
                    numeric_stack.pop();
                    float b = numeric_stack.top();
                    numeric_stack.pop();
                    float c = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(a);
                    numeric_stack.push(c);
                    numeric_stack.push(b);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `ROT` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_NROT)
            {
                if (numeric_stack.size() > 2)
                {
                    float c = numeric_stack.top();
                    numeric_stack.pop();
                    float b = numeric_stack.top();
                    numeric_stack.pop();
                    float a = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(c);
                    numeric_stack.push(a);
                    numeric_stack.push(b);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `-ROT` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_NIP)
            {
                if (numeric_stack.size() > 1)
                {
                    int b = numeric_stack.top();
                    numeric_stack.pop();
                    numeric_stack.pop();
                    numeric_stack.push(b);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `NIP` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_PER)
            {
                if (numeric_stack.size() > 1)
                {
                    int a = numeric_stack.top();
                    numeric_stack.pop();
                    int b = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(b % a);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `MODULO` OPERATOR" << endl;
                    break;
                }
            }

            // TODO: Underconstruction
            if (token_list[j].type == P_FUNC_PAUSE)
            {
                char c;
                cin >> c;
            }

            if (token_list[j].type == P_FUNC_EXIT)
            {
                if (numeric_stack.size() > 0)
                {
                    int errcode = numeric_stack.top();
                    numeric_stack.pop();
                    exit(errcode);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: no exit-code provided for `exit` SYSCALL" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_ADDR32)
            {
                numeric_stack.push(static_cast<int>(free_ptr_32));
            }

            if (token_list[j].type == P_DEREF32)
            {
                if (!numeric_stack.empty())
                {
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(byte32Memory[loc]);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: no `PTR` provided to dereference from `I32` memory" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_PUSH32)
            {
                if (numeric_stack.size() > 1)
                {
                    int data = numeric_stack.top();
                    numeric_stack.pop();
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    byte32Memory[loc] = data;
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a `PTR` and `DATA` to store to `I32` memory" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_ADDR16)
            {
                numeric_stack.push(static_cast<int>(free_ptr_16));
            }

            if (token_list[j].type == P_DEREF16)
            {
                if (!numeric_stack.empty())
                {
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(byte16Memory[loc]);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: no `PTR` provided to dereference from `I16` memory" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_PUSH16)
            {
                if (numeric_stack.size() > 1)
                {
                    int data = numeric_stack.top();
                    numeric_stack.pop();
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    byte16Memory[loc] = data;
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a `PTR` and `DATA` to store to `I16` memory" << endl;
                    break;
                }
            }
            // TODO: Underwork

            if (token_list[j].type == P_ADDR8)
            {
                numeric_stack.push(static_cast<int>(free_ptr));
            }

            if (token_list[j].type == P_DEREF8)
            {
                if (!numeric_stack.empty())
                {
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    numeric_stack.push(static_cast<int>(memory[loc]));
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: no `PTR` provided to dereference from `I8` memory" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_PUSH8)
            {
                if (numeric_stack.size() > 1)
                {
                    int data = numeric_stack.top();
                    numeric_stack.pop();
                    int loc = numeric_stack.top();
                    numeric_stack.pop();

                    memory[loc] = data & 0xFF;
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a `PTR` and `DATA` to store to `I8` memory" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_PUTS)
            {
                if (numeric_stack.size() > 1)
                {
                    int end = numeric_stack.top();
                    numeric_stack.pop();
                    int begin = numeric_stack.top();
                    numeric_stack.pop();

                    for (size_t j = static_cast<size_t>(begin); j < static_cast<size_t>(end); j++)
                        printf("%c", memory[j]);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[int: begin int: end] puts`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_PUTC)
            {
                if (numeric_stack.size() > 0)
                {
                    printf("%c", memory[numeric_stack.top()]);
                    numeric_stack.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[int: loc] putc`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_WRITELINE)
            {
                if (c_string.size() > 0)
                {
                    cout << c_string.top();
                    c_string.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[str: data] writeline`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_STRDUP)
            {
                if (c_string.size() > 0)
                {
                    c_string.push(c_string.top());
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[str: data] strdup`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_CSTRDROP)
            {
                if (c_string.size() > 0)
                {
                    c_string.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provieded to `STRDROP` manipulator" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_CSTRLEN)
            {
                if (c_string.size() > 0)
                {
                    numeric_stack.push(c_string.top().size());
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[str: data] cstrlen`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_ARGC)
            {
                numeric_stack.push(argc);
            }

            if (token_list[j].type == P_ARGV)
            {
                if (numeric_stack.size() > 0)
                {
                    if(numeric_stack.top() >= 0 && numeric_stack.top() < argc){
                        c_string.push(argv[numeric_stack.top()]);
                        numeric_stack.pop();
                    }
                    else{
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: trying to read out of bounds" << endl;
                        break;
                    }
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments provided to `ARGV` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_CSTR_TO_STR)
            {
                if (c_string.size() > 0)
                {
                    int begin = free_ptr;
                    int end = c_string.top().size();

                    for (size_t i = 0; i < end; i++)
                        memory[begin++] = c_string.top()[i];
                    c_string.pop();
                    numeric_stack.push(free_ptr);
                    free_ptr = begin;
                    numeric_stack.push(free_ptr + end);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[str: data] cstr-to-str`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_STRCMP)
            {
                if (c_string.size() > 1)
                {
                    string B = c_string.top();
                    c_string.pop();
                    string A = c_string.top();
                    c_string.pop();
                    numeric_stack.push(B == A);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[str: data str: data] strcmp`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_STR_TO_CSTR)
            {
                if (numeric_stack.size() > 1)
                {
                    int end = numeric_stack.top();
                    numeric_stack.pop();
                    int begin = numeric_stack.top();
                    numeric_stack.pop();

                    string load_str_from_mem = "";
                    for (; begin <= end; begin++)
                    {
                        load_str_from_mem += memory[begin];
                    }
                    c_string.push(load_str_from_mem);
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function `[int: begin int: end] str-to-cstr`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_FUNC_WRITE)
            {
                if (numeric_stack.size() > 3)
                {
                    int syscallnum = numeric_stack.top();
                    numeric_stack.pop();

                    int arg1 = numeric_stack.top();
                    numeric_stack.pop();

                    int arg2 = numeric_stack.top();
                    numeric_stack.pop();

                    int arg3 = numeric_stack.top();
                    numeric_stack.pop();

                    if (syscallnum == 1)
                    {
                        if (arg1 == 1)
                        {
                            for (int p = arg2; p < arg2 + arg3; p++)
                                cout << memory[p];
                        }
                        else if (arg1 == 2)
                        {
                            for (int p = arg2; p < arg2 + arg3; p++)
                                cerr << memory[p];
                        }
                        else
                        {
                            cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unknown FileDescriptor provided  " << arg1 << " to function [int: arg3 int: arg2 int: arg1 int: syscall] write" << endl;
                            break;
                        }
                    }
                    else
                    {
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unknown syscall number provided  " << syscallnum << " to function [int: arg3 int: arg2 int: arg1 int: syscall] write" << endl;
                        break;
                    }
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: too few arguments to function [int: arg3 int: arg2 int: arg1 int: syscall] write" << endl;
                    break;
                }
            }
            // TODO: Underconstruction

            if (token_list[j].type == P_IF)
            {
                int location = EXPECT_THEN(j, token_list, size);
                ptoken.push(P_IF);

                if (location == -1)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: incomplete `IF` statement definition expected a `then`" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_THEN)
            {
                if (!ptoken.empty() && (ptoken.top() == P_IF || ptoken.top() == P_WHILE))
                {
                    if (ptoken.top() == P_WHILE)
                    {
                        wcon.push(static_cast<bool>(numeric_stack.top()));
                    }
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unexpected occurence of `then` keyword" << endl;
                    break;
                }

                if (numeric_stack.size() > 0)
                {
                    int jmp = EXPECT_END(j, token_list, size, ptoken);
                    int jmp2 = EXPECT_ONLY_END(j, token_list, size);

                    if (jmp == -1 || jmp2 == -1)
                    {
                        if (ptoken.top() == P_IF)
                        {
                            cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected an `END` of this `if` statement" << endl;
                        }
                        else if (ptoken.top() == P_WHILE)
                        {
                            cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected an `END` of this `while` statement" << endl;
                        }
                        break;
                    }

                    CondEncounter = numeric_stack.top();
                    numeric_stack.pop();

                    if (CondEncounter == true)
                    {
                    }
                    else if (CondEncounter == false)
                    {
                        j = jmp;
                    }
                    else
                    {
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a `BOOL` but found `INT`" << endl;
                        break;
                    }
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a `BOOL` but found `NONE`" << endl;
                    break;
                }
            }
            if (token_list[j].type == P_ELIF)
            {
                if (!ptoken.empty() && ptoken.top() == P_IF)
                {
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unexpected occurence of `elif` keyword" << endl;
                    break;
                }

                int jmp = EXPECT_END(j, token_list, size, ptoken);
                int location = EXPECT_THEN(j, token_list, size);

                if (location == -1)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: incomplete `ELIF` statement definition expected a `then`" << endl;
                    break;
                }

                if (jmp == -1)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected an `END` of this `elif` statement" << endl;
                    break;
                }

                if (CondEncounter == false)
                {
                    CondEncounter = true;
                }
                else if (CondEncounter == true)
                {
                    j = EXPECT_ONLY_END(j, token_list, size);
                }
            }
            if (token_list[j].type == P_ELSE)
            {
                if (!ptoken.empty() && ptoken.top() == P_IF)
                {
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unexpected occurence of `else` keyword" << endl;
                    break;
                }

                if (CondEncounter == false)
                {
                    CondEncounter = true;
                }
                else if (CondEncounter == true)
                {
                    j = EXPECT_ONLY_END(j, token_list, size);
                }
            }

            // TODO: Underconst
            if (token_list[j].type == P_WORD)
            {
                bool isValid = false;
                struct Tuple<string, int, Pitt_token> def_loc =
                {
                };
                for (auto k : definition)
                {
                    if (k.first == token_list[j].token)
                    {
                        def_loc = k;
                        isValid = true;
                        break;
                    }
                }

                if (!isValid)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unknown token found `" << token_list[j].token << "`" << endl;
                    exit(1);
                }
                else
                {
                    return_stack.push(j);
                    j = def_loc.second;
                    ptoken.push(P_DEF);
                }
            }

            if (token_list[j].type == P_DEF)
            {
                inDef = true;
                ptoken.push(P_DEF);

                if (token_list[j + 1].type == P_WORD)
                {
                    int begVerifier = EXPECT_ONLY_BEGIN(j, token_list, size);

                    if (begVerifier == -1)
                    {
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected the body of the label to be marked with `begin` KEYWORD" << endl;
                        break;
                    }

                    j++;
                    bool isCopy = false;
                    struct Tuple<string, int, Pitt_token> firstDef =
                    {
                    };

                    for (auto p : definition)
                    {
                        if (p.first == token_list[j].token)
                        {
                            firstDef = p;
                            isCopy = true;
                            break;
                        }
                    }

                    if (isCopy)
                    {
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: redefinition of label `" << token_list[j].token << "`" << endl;
                        cout << filename << ":" << firstDef.third.r << ":" << firstDef.third.c << ": NOTE: first declared here" << endl;
                        break;
                    }
                    else
                    {
                        definition[tp].first = token_list[j].token;
                        definition[tp].second = begVerifier;
                        definition[tp].third = token_list[j];
                        tp++;
                    }
                }
                else
                {
                    if (token_list[j + 1].type != P_WORD && token_list[j + 1].type != P_EOT)
                    {
                        cout << filename << ":" << token_list[j + 1].r << ":" << token_list[j + 1].c << ": ERROR: expected the label name to be `WORD` but found `" << usable_names[token_list[j + 1].type] << ": " << token_list[j + 1].token << "`" << endl;
                        break;
                    }
                    if (token_list[j + 1].type == P_EOT)
                    {
                        cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected a name of this label but found `NONE`" << endl;
                        break;
                    }
                }
            }

            if (token_list[j].type == P_BEGIN)
            {
                if (!ptoken.empty())
                {
                    if (ptoken.top() == P_DEF)
                    {
                        int endVerifier = EXPECT_ONLY_END(j, token_list, size);
                        if (endVerifier == -1)
                        {
                            cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: expected an `END` of this label" << endl;
                            break;
                        }

                        end_behaviour.push(inDef);
                        if (inDef == true)
                        {
                            inDef = false;
                            j = endVerifier;
                        }
                    }
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unexpected occurence of `begin` keyword" << endl;
                    break;
                }
            }
            // TODO: Underconst

            if (token_list[j].type == P_END)
            {
                if (!ptoken.empty() && (ptoken.top() == P_IF || ptoken.top() == P_WHILE || ptoken.top() == P_DEF))
                {
                    if (ptoken.top() == P_WHILE)
                    {
                        if (!wcon.empty() && !wloc.empty())
                        {
                            if (wcon.top() == true)
                            {
                                j = wloc.top();
                            }
                            wcon.pop();
                            wloc.pop();
                        }
                    }

                    if (ptoken.top() == P_DEF)
                    {
                        if (!return_stack.empty())
                        {
                            if (end_behaviour.top() == false || end_behaviour.empty())
                            {
                                j = return_stack.top();
                                return_stack.pop();
                            }
                            else
                            {
                                end_behaviour.pop();
                            }
                        }
                    }

                    ptoken.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: unexpected occurence of `end` keyword" << endl;
                    break;
                }
            }

            if (token_list[j].type == P_WHILE)
            {
                int location = EXPECT_THEN(j, token_list, size);

                if (location == -1)
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: incomplete `WHILE` statement definition expected a `then`" << endl;
                    break;
                }

                ptoken.push(P_WHILE);
                wloc.push(j);
            }

            if (token_list[j].type == P_FUNC_PRINT)
            {
                if (!numeric_stack.empty())
                {
                    cout << numeric_stack.top();
                    numeric_stack.pop();
                }
                else
                {
                    cout << filename << ":" << token_list[j].r << ":" << token_list[j].c << ": ERROR: Too few arguments to function `[int: data] print`" << endl;
                    break;
                }
            }
        }
        j += 1;
    }
    // TODO: WE'LL BE DOING TYPE-CHECK SO THIS WILL BE SHIFTED THERE
    if (numeric_stack.size() > 0)
    {
        cout << filename << ":" << token_list[j - 1].r << ":" << token_list[j - 1].c << ": ERROR: unhandled data on stack <ElementType.INT:" << numeric_stack.size() << ">" << endl;
        exit(0);
    }
}
void dump_file(const char *filename, const Pitt_token *token_list)
{
    printf("[INFO] Reading: `%s`\n\n", filename);
    for (size_t j = 0; j < words_count(filename); j++)
    {
        cout << "<name:" << token_list[j].token << ",type:" << token_list[j].type << ",line:" << token_list[j].r << ",addr:" << &token_list[j] << ">" << endl;
    }
}

int main(int args, char **argv)
{
    [args, argv]()
    {
        if (args < 3)
        {
            printf("pitt:\033[0;31m fatal error:\033[0;37m no input files or operation type provided!\n");
            help();
        }
        else
        {
            init_map();
            // Filename :- argv[2]
            // Command :- argv[1]
            check_file_ext(argv[2]);
            check_file_val(argv[2]);

            if (strcmp(argv[1], "build") == 0)
            {
                printf("[INFO] Simulating file: `%s`\n\n", argv[2]);
                Pitt_token *tokens = new Pitt_token[words_count(argv[2])];
                for (size_t j = 0; j < words_count(argv[2]); j++)
                    tokens[j].type = P_EOT;
                parse(argv[2], tokens);
                simulate_file(argv[2], tokens, args, argv);
            }
            else if (strcmp(argv[1], "dump") == 0)
            {
                Pitt_token *tokens = new Pitt_token[words_count(argv[2])];
                parse(argv[2], tokens);
                dump_file(argv[2], tokens);
            }
            else
            {
                printf("pitt:\033[0;31m fatal error:\033[0;37m invalid operation type provided `%s`\n", argv[1]);
                help();
            }
        }
    }();
    return 0;
}