#include <bits/stdc++.h>
//#include <boost/algorithm/string.hpp>
#include <regex>

using namespace std;

///////////////// HELPER FUNCTION //////////////////////////
//Integer Statement parser
void declaration_parser(string &code,regex pattern,vector<string> &parsed_stmts){
	//parser for integer declaration
	smatch match_itr;
	string str="";

	//Parsing the code greedily to get the smallest match
	do{
		str="";
		//Searching the location of the pattern
		regex_search(code,match_itr,pattern);
		//Appending the pattern to the temporary string
		for (auto x : match_itr){
			str+=x;
		}

		//Adding the statement to the vector of declaration
		if(str.size()!=0){
			parsed_stmts.push_back(str);
		}
		code.replace(code.find(str),str.length(),"");
	}while(str!="");
	//cout<<code<<endl;
}
void remove_spaces_semicol(string &str){
	string new_str;
	for(unsigned int i=0;i<str.size();i++){
		if(str[i]!=' ' && str[i]!=';'){
			new_str+=str[i];
		}
	}
	str=new_str;
}

//Filling symbol Table
void fill_symbol_table(vector<string> &parsed_stmts,
			vector<tuple<string,string,int,int>> &symbol_table){

	//cout<<endl<<"Creating the symbol Table"<<endl;
	//Defining the regex pattern for parsing dtype
	smatch match_itr;	//the iterator for the matching pos
	regex pointer_pattern("(int|float|struct[\\s]+[\\S]+)[*]");
	regex normal_pattern("(int|float|struct[\\s]+[\\S]+)");

	//Starting the interation one by one over all declaration stmts
	for(unsigned int stm_i=0;stm_i<parsed_stmts.size();stm_i++){
		string stmt=parsed_stmts[stm_i];
		//cout<<stmt<<endl;
		//Classifying between two type os writing pointer
		int pointer_count=0;
		string var_type;
		if(regex_search(stmt,match_itr,pointer_pattern)){
			pointer_count=1;
			//cout<<match_itr[1]<<endl;
		}
		else{
			regex_search(stmt,match_itr,normal_pattern);
			//cout<<match_itr[1]<<endl;
		}

		//Now breaking the varaibles assigning them to table
		//Cleaning up the string of type specifier
		var_type=match_itr[1];
		stmt.replace(stmt.find(match_itr[0]),match_itr[0].length(),"");
		remove_spaces_semicol(stmt);//not necessary could have been dealt below
		//cout<<stmt<<endl;

		//Traversing the string to get the variables
		string var_name="";
		int var_pointer_count=pointer_count;
		int arr_size=0;
		for(unsigned int i=0;i<stmt.length();i++){
			//if more *s are coming
			if(stmt[i]=='*'){
				var_pointer_count++;
				continue;
			}
			//extracting the varaible name
			while(stmt[i]!=',' && stmt[i]!='[' && i<stmt.length()){
				var_name+=stmt[i];
				i++;
			}
			//Handling the array size count
			if(stmt[i]=='['){
				i++;//skipping the current bracket
				while(stmt[i]!=']'){
					arr_size=arr_size*10+(stmt[i]-'0');
					i++;
				}
				i+=1;//going over the ']' symbol
			}
			//Handling the line number for internal name equivalence
			//two variable declared on same line will have same character
			var_name+='0'+stm_i;

			//cout<<var_type<<" "<<var_name<<" "<<var_pointer_count<<" "<<arr_size<<endl;
			//Saving the varaible in the symbol tuple
			symbol_table.push_back(make_tuple(var_type,var_name,var_pointer_count,arr_size));
			//Restoring for next variable
			var_name="";
			var_pointer_count=pointer_count;
			arr_size=0;
		}
		//cout<<endl;
	}
}

//Creating the info vector based on the declaration of struct
vector<pair<string,vector<tuple<string,string,int,int>>>> create_struct_info_vec(\
										vector<string> parsed_defn,\
										string saving_preference){
	cout<<endl<<"Creating the struct definition table"<<endl;
	//Grouping the name of struct and internal definitions
	regex grp_struct_pattern("(struct.*)[{](.*)[}];");
	regex stmt_parser_pattern("(?:int|float|struct).*?;");
	smatch match_itr;

	vector<pair<string,vector<tuple<string,string,int,int>>>> struct_defn_table;
	//Now starting to iterate over all the definition of struct
	for(unsigned int i=0;i<parsed_defn.size();i++){
		string defn_stmt=parsed_defn[i];
		//Extracting out the groups in the definition
		regex_search(defn_stmt,match_itr,grp_struct_pattern);
		string snames=(string)match_itr[1];
		string dec_stmts=(string)match_itr[2];

		//Creating the statement vector from sequence of declaration
		vector<string> dec_stmts_vector;
		declaration_parser(dec_stmts,stmt_parser_pattern,dec_stmts_vector);
		// cout<<"Parserd declaration statements"<<endl;
		// for(unsigned int i=0;i<dec_stmts_vector.size();i++){
		// 	cout<<dec_stmts_vector[i]<<endl;
		// }

		//Now creating the symbol table for the curretn definition
		vector<tuple<string,string,int,int>> defn_table;
		fill_symbol_table(dec_stmts_vector,defn_table);
		struct_defn_table.push_back(make_pair(snames,defn_table));
	}
	return struct_defn_table;
}

//Creating the info vector based on the declaration of struct
vector<pair<pair<string,string>,vector<tuple<string,string,int,int>>>> create_func_info_vec(\
                                                                    vector<string> parsed_defn){
	cout<<endl<<"Creating the func definition table"<<endl;
	//Grouping the name of struct and internal definitions
	regex grp_struct_pattern("(int[\\s|\\*]+|float[\\s|\\*]+|struct[\\s]+[\\S]+[\\s|\\*]+)(.*)[\\(](.*?)[\\)];");
	regex stmt_parser_pattern("(?:int|float|struct).*?,");
	smatch match_itr;

	vector<pair<pair<string,string>,vector<tuple<string,string,int,int>>>> struct_defn_table;
	//Now starting to iterate over all the definition of struct
	for(unsigned int i=0;i<parsed_defn.size();i++){
		string defn_stmt=parsed_defn[i];
		//Extracting out the groups in the definition
		regex_search(defn_stmt,match_itr,grp_struct_pattern);

		//Extracting the return type of function
		string stype=(string)match_itr[1];
		//Calculating the degree of pointer in return type
		int degree_ptr_return=0;
		for(unsigned int j=stype.length()-1;j>=0;j--){
			if(stype[j]=='*'){
				degree_ptr_return++;
			}
			else if(stype[j]!=' '){
				break;
			}
			//erasing the space or star from the string name
			stype.erase(j,1);
		}
		stype+='0'+degree_ptr_return;
		//cout<<stype<<endl;

		//Extracting the name of function
		string snames=(string)match_itr[2];
		//cout<<snames<<endl;

		//Extracting the arguments
		string dec_stmts=(string)match_itr[3];
		dec_stmts+=',';
		//cout<<dec_stmts<<endl;

		//Creating the statement vector from sequence of declaration
		vector<string> dec_stmts_vector;
		declaration_parser(dec_stmts,stmt_parser_pattern,dec_stmts_vector);
		// cout<<"Parserd declaration statements"<<endl;
		// for(unsigned int i=0;i<dec_stmts_vector.size();i++){
		// 	cout<<dec_stmts_vector[i]<<endl;
		// }

		//Now creating the symbol table for the curretn definition
		vector<tuple<string,string,int,int>> defn_table;
		fill_symbol_table(dec_stmts_vector,defn_table);
		pair<string,string> name_rettype=make_pair(stype,snames);
		struct_defn_table.push_back(make_pair(name_rettype,defn_table));
	}
	return struct_defn_table;
}

/////Function to create the symbol table for structures////////
vector<vector<int>> struct_equi(vector<pair<string,vector<tuple<string,string,int,int>>>> &input)
{
    //cout<<"hello\n";
    int n=input.size();
    //cout<<n<<endl;
    vector<vector<int>> equi_table(n,vector<int>(n,-1));
    int i,j,k,l;
    for(i=0;i<n;i++)
    {
        equi_table[i][i]=1;
    }
    /*for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            cout<<equi_table[i][j]<<" ";
        }
        cout<<endl;
    }*/
    vector<tuple<string,string,int,int>> tup1=input[0].second;
    //cout<<get<0>(tup1[0])<<endl;
    int flag=1;
    while(flag)
    {
        flag=0;
        for(i=0;i<n;i++)
        {
            for(j=i+1;j<n;j++)
            {
                if(equi_table[i][j]!=-1)
                {
                    continue;
                }
                vector<tuple<string,string,int,int>> v1=input[i].second;
                vector<tuple<string,string,int,int>> v2=input[j].second;
                if(v1.size()!=v2.size())
                {
                    equi_table[i][j]=0;
                    flag=1;
                    continue;
                }
                for(k=0;k<v1.size();k++)
                {
                    string type1=get<0>(v1[k]);
                    string type2=get<0>(v2[k]);
                    if(type1.find("struct")!=string::npos&&type2.find("struct")!=string::npos)
                    {
                        int pos1,pos2;
                        for(l=0;l<n;l++)
                        {
                            if(input[l].first==type1)
                            {
                                pos1=l;
                                break;
                            }
                        }
                        for(l=0;l<n;l++)
                        {
                            if(input[l].first==type2)
                            {
                                pos2=l;
                                break;
                            }
                        }
                        if(pos1>pos2)
                        {
                            swap(pos1,pos2);
                        }
                        if(equi_table[pos1][pos2]==0)
                        {
                            equi_table[i][j]=0;
                            flag=1;
                            break;
                        }
                        if(equi_table[pos1][pos2]==-1)
                        {
                            break;
                        }
                    }
                    else if(type1.find("struct")==string::npos&&type2.find("struct")==string::npos)
                    {
                        if(!(get<0>(v1[k])==get<0>(v2[k])&&get<2>(v1[k])==get<2>(v2[k])&&get<3>(v1[k])==get<3>(v2[k])))
                        {
                            equi_table[i][j]=0;
                            flag=1;
                            break;
                        }
                    }
                    else
                    {
                        equi_table[i][j]=0;
                        flag=1;
                        break;
                    }
                }
                if(k==v1.size())
                {
                    equi_table[i][j]=1;
                    flag=1;
                }
            }
        }
    }
    return equi_table;
}

///////////CREATING NAME EQUIVALENCE TABLE//////////////////
vector<vector<int>> name_equi(vector<tuple<string,string,int,int>>& symtab)
{
    int n=symtab.size(),i,j;
    vector<vector<int>> name_equi_table(n,vector<int>(n,0));
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(get<2>(symtab[i])==0&&get<2>(symtab[j])==0&&get<3>(symtab[i])==0&&get<3>(symtab[j])==0)
            {
                string type1=get<0>(symtab[i]);
                string type2=get<0>(symtab[j]);
                if(type1.find("struct")==string::npos&&type2.find("struct")==string::npos&&type1==type2)
                {
                    name_equi_table[i][j]=1;
                }
            }
        }
    }
    return name_equi_table;
}
/////////CREATING STRUCTURE EQUIVALENCE TABLE///////////////
void struc_equivalence(vector<vector<int>>&stru_equi_table,vector<tuple<string,string,int,int>>& symtab,map<string,int>&m,vector<vector<int>>&equi_table)
{
    int i,j,n=stru_equi_table.size();
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(stru_equi_table[i][j]==0)
            {
                string type1=get<0>(symtab[i]);
                string type2=get<0>(symtab[j]);
                if(type1.find("struct")!=string::npos&&type2.find("struct")!=string::npos)
                {
                    if(equi_table[m[type1]][m[type2]]==1||equi_table[m[type2]][m[type1]]==1)
                    {
                        if(get<2>(symtab[i])==get<2>(symtab[j])&&get<3>(symtab[i])==get<3>(symtab[j]))
                        {
                            stru_equi_table[i][j]=1;
                        }
                    }
                }
                else
                {
                    if(type1==type2&&get<2>(symtab[i])==get<2>(symtab[j])&&get<3>(symtab[i])==get<3>(symtab[j]))
                    {
                        stru_equi_table[i][j]=1;
                    }
                }
            }
        }
    }
}

/////////CREATING FUNCTION EQUIVALENCE TABLE////////////////////////
vector<vector<int>> func_equi(vector<pair<pair<string,string>,vector<tuple<string,string,int,int>>>> func_defn_table,vector<vector<int>>equi_table,map<string,int>&m)
{
    int n=func_defn_table.size(),i,j;
    vector<vector<int>> func_equi_table(n,vector<int>(n,0));
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            string type1=(func_defn_table[i].first).first;
            string type2=(func_defn_table[j].first).first;
            if(type1.find("struct")!=string::npos&&type2.find("struct")!=string::npos)
            {
                int num1=0,num2=0,l;
                for(l=type1.length()-1;l>=0;l--)
                {
                    if(!(type1[l]>='0'&&type1[l]<='9'))
                    {
                        break;
                    }
                    num1=num1*10+(type1[l]-'0');
                    type1.erase(l,1);
                }
                for(l=type2.length()-1;l>=0;l--)
                {
                    if(!(type2[l]>='0'&&type2[l]<='9'))
                    {
                        break;
                    }
                    num2=num2*10+(type2[l]-'0');
                    type2.erase(l,1);
                }
                //cout<<num1<<" "<<type1<<" "<<num2<<" "<<type2<<endl;
                if(num1==num2&&(equi_table[m[type1]][m[type2]]==1||equi_table[m[type2]][m[type1]]==1))
                {
                    vector<tuple<string,string,int,int>> v1=func_defn_table[i].second;
                    vector<tuple<string,string,int,int>> v2=func_defn_table[j].second;
                    if(v1.size()==v2.size())
                    {
                        unsigned int k;
                        for(k=0;k<v1.size();k++)
                        {
                            if(get<2>(v1[k])==get<2>(v2[k]))
                            {
                                string t1=get<0>(v1[k]);
                                string t2=get<0>(v2[k]);
                                if(t1.find("struct")!=string::npos&&t2.find("struct")!=string::npos)
                                {
                                    if(equi_table[m[t1]][m[t2]]==1||equi_table[m[t2]][m[t1]]==1)
                                    {
                                        func_equi_table[i][j]=1;
                                    }
                                    else if(t1.find("struct")==string::npos&&t2.find("struct")==string::npos)
                                    {
                                        if(t1==t2)
                                        {
                                            func_equi_table[i][j]=1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(type1.find("struct")==string::npos&&type2.find("struct")==string::npos)
            {
                if(type1!=type2)
                {
                    continue;
                }
                else
                {
                    vector<tuple<string,string,int,int>> v1=func_defn_table[i].second;
                    vector<tuple<string,string,int,int>> v2=func_defn_table[j].second;
                    if(v1.size()==v2.size())
                    {
                        unsigned int k;
                        for(k=0;k<v1.size();k++)
                        {
                            if(get<2>(v1[k])==get<2>(v2[k]))
                            {
                                string t1=get<0>(v1[k]);
                                string t2=get<0>(v2[k]);
                                if(t1.find("struct")!=string::npos&&t2.find("struct")!=string::npos)
                                {
                                    if(equi_table[m[t1]][m[t2]]==1||equi_table[m[t2]][m[t1]]==1)
                                    {
                                        func_equi_table[i][j]=1;
                                    }
                                    else if(t1.find("struct")==string::npos&&t2.find("struct")==string::npos)
                                    {
                                        if(t1==t2)
                                        {
                                            func_equi_table[i][j]=1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return func_equi_table;
}
////////////////////////creating internal equivalence table/////////////////
vector<vector<int>> internal(vector<tuple<string,string,int,int>> symbol_table,map<string,int>&m,vector<vector<int>>&struct_equi)
{
    int i,j,n=symbol_table.size(),k;
    vector<vector<int>> internal_equi(n,vector<int>(n,0));
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(get<3>(symbol_table[i])==get<3>(symbol_table[j])&&get<3>(symbol_table[i])!=0&&get<2>(symbol_table[i])==get<2>(symbol_table[j]))
            {
                int num1=0,num2=0;
                string name1=get<1>(symbol_table[i]);
                string name2=get<1>(symbol_table[j]);
                for(k=name1.length()-1;k>=0;k--)
                {
                    if(!(name1[k]>='0'&&name1[k]<='9'))
                    {
                        break;
                    }
                    num1=(num1*10+(name1[k]-'0'));
                }
                for(k=name2.length()-1;k>=0;k--)
                {
                    if(!(name2[k]>='0'&&name2[k]<='9'))
                    {
                        break;
                    }
                    num2=(num2*10+(name2[k]-'0'));
                }
                if(num1==num2)
                {
                    string type1=get<0>(symbol_table[i]);
                    string type2=get<0>(symbol_table[j]);
                    if(type1.find("struct")==string::npos&&type2.find("struct")==string::npos)
                    {
                        internal_equi[i][j]=1;
                    }
                    else if(type1.find("struct")!=string::npos&&type2.find("struct")!=string::npos)
                    {
                        if(struct_equi[m[type1]][m[type2]]==1||struct_equi[m[type2]][m[type1]]==1)
                        {
                            internal_equi[i][j]=1;
                        }
                    }
                }
            }
        }
    }
    return internal_equi;
}
///////////////// MAIN FUNCTION ////////////////////////////
//file reader Reader function
string read_code_from_file(string filename){
	ifstream code_file(filename);
	string code;
	string line;
	if(code_file.is_open()){
		cout<<"Reading the code file"<<endl<<endl;
		while(getline(code_file,line)){
			code+=line+'\n';
		}
		code_file.close();
	}
	else{
		cout<<"Unable to open the file\n";
	}

	return code;
}

void do_BFS(int start,vector<vector<int>> &table,\
			vector<tuple<string,string,int,int>> &symbol_table,\
			int visited[]){
	int size=table.size();
	cout<<get<1>(symbol_table[start])<<",";
	for(int i=start+1;i<size;i++){
		if(visited[i]==0 && table[start][i]==1){
			// cout<<get<1>(symbol_table[i])<<",";
			visited[i]=1;
			do_BFS(i,table,symbol_table,visited);
		}
	}
}

void print_equivalence(vector<vector<int>> &table,\
			vector<tuple<string,string,int,int>> &symbol_table){
	//To show the name of all the varaibles which are name equivalent
	//Initializing the visited array
	int num=symbol_table.size();
	int visited[num];
	for(int i=0;i<num;i++){
		visited[i]=0;
	}

	//Doing DFS
	for(int i=0;i<num;i++){
		if(visited[i]==0){
			cout<<"[ ";
			visited[i]=1;
			do_BFS(i,table,symbol_table,visited);
			cout<<" ]"<<endl;
		}
	}
	cout<<"##########################################"<<endl;
}

void print_equivalence_table(vector<vector<int>> &table){
	int n=table.size();
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			cout<<table[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

int main(int argc, char** argv){
	// string code="erferf int a,b[100],c5; float c,d,f;  abhinabddefvefv int a1,a2,a3; int abiefvjef;";
	// string code2="	int* * func1(int a,float b,struct node cq);\
	// 				struct node* * * func2(int a,int b,struct node1 c1);\
	// 				bbb int i,j,*j[15],*k[15];\
	// 				struct node{int a;float b;struct node c1;};\
	// 				struct node1{int a;float b;struct node c2;};\
	// 				struct node* *n1,*n2,n3  [200];\
	// 				struct node1* *n4;\
	// 				";
	string filename="code.txt";
	string code2=read_code_from_file(filename);
	//cout<<code2<<endl<<endl;

	//Regex-Patterns for interger and float parsing
	regex int_pattern("int.*?;");
	regex float_pattern("float.*?;");
	regex struct_defn_pattern("struct.*?[{].*?[}];");
	regex struct_dec_pattern("struct.*?;");
	//Regex-Pattern for function extraction
	regex func_dec_pattern("(?:int|float|struct).*?[\\(].*?[\\)];");

	//Parsing the declaration statements form the whole code
	vector<string> parsed_stmts;//this will hold the statements
	vector<string> parsed_defn;//this will hold the definitions
	vector<string> parsed_func;//this will hold the func declaration


	declaration_parser(code2,func_dec_pattern,parsed_func);
	declaration_parser(code2,struct_defn_pattern,parsed_defn);
	declaration_parser(code2,struct_dec_pattern,parsed_stmts);
	declaration_parser(code2,int_pattern,parsed_stmts);
	declaration_parser(code2,float_pattern,parsed_stmts);

	//Printing the parsed statements and declarations
	cout<<"Parserd declaration statements"<<endl;
	for(unsigned int i=0;i<parsed_stmts.size();i++){
		cout<<parsed_stmts[i]<<endl;
	}
	cout<<endl<<"Parsed definition statements"<<endl;
	for(unsigned int i=0;i<parsed_defn.size();i++){
		cout<<parsed_defn[i]<<endl;
	}
	cout<<endl<<"Parsed function definitions"<<endl;
	for(unsigned int i=0;i<parsed_func.size();i++){
		cout<<parsed_func[i]<<endl;
	}

	//Making the symbol table from the parsed statements
	cout<<"##############################################"<<endl;
	vector<tuple<string,string,int,int>> symbol_table;
	fill_symbol_table(parsed_stmts,symbol_table);
	//Priingng the symbol table
	cout<<endl<<"Prinitng the symbol table"<<endl;
	cout<<"SNo.\t\t"<<"Type\t\t"<<"Name\t\t"<<"Pointer\t\t"<<"ArraySize"<<endl;
	for(unsigned int i=0;i<symbol_table.size();i++){
		cout<<i<<":\t";
		cout<<get<0>(symbol_table[i])<<"\t\t";
		cout<<get<1>(symbol_table[i])<<"\t\t";
		cout<<get<2>(symbol_table[i])<<"\t\t";
		cout<<get<3>(symbol_table[i])<<"\t\t";
		cout<<endl;
	}

	//Creating the representation of structs defn for checking equivalence
	cout<<"##############################################"<<endl;
	vector<pair<string,vector<tuple<string,string,int,int>>>> struct_defn_table;
	struct_defn_table=create_struct_info_vec(parsed_defn,"packed");
	//Printing the structure definition table
	for(unsigned int i=0;i<struct_defn_table.size();i++){
		cout<<i<<" :"<<struct_defn_table[i].first<<endl;
		vector<tuple<string,string,int,int>> struct_symbol_table;
		struct_symbol_table=struct_defn_table[i].second;
		for(unsigned int j=0;j<struct_symbol_table.size();j++){
			cout<<get<0>(struct_symbol_table[j])<<"\t\t";
			cout<<get<1>(struct_symbol_table[j])<<"\t\t";
			cout<<get<2>(struct_symbol_table[j])<<"\t\t";
			cout<<get<3>(struct_symbol_table[j])<<"\t\t";
			cout<<endl;
		}
		cout<<endl;
	}

	//Creating the function definition table similar to struct
	cout<<"##############################################"<<endl;
	vector<pair<pair<string,string>,vector<tuple<string,string,int,int>>>> func_defn_table;
    func_defn_table=create_func_info_vec(parsed_func);
    //Printing the function definition table
	cout<<"Printing the function definition"<<endl;
	for(unsigned int i=0;i<func_defn_table.size();i++){
	    string stype=(func_defn_table[i].first).first;
	    string sname=(func_defn_table[i].first).second;
		cout<<i<<" : "<<sname<<" "<<stype<<endl;
		vector<tuple<string,string,int,int>> func_symbol_table;
		func_symbol_table=func_defn_table[i].second;
		for(unsigned int j=0;j<func_symbol_table.size();j++){
			cout<<get<0>(func_symbol_table[j])<<"\t\t";
			cout<<get<1>(func_symbol_table[j])<<"\t\t";
			cout<<get<2>(func_symbol_table[j])<<"\t\t";
			cout<<get<3>(func_symbol_table[j])<<"\t\t";
			cout<<endl;
		}
		cout<<endl;
	}
	///////////////////creating map for structure name///////////////////////
    map<string,int> m;
    for(unsigned int i=0;i<struct_defn_table.size();i++)
    {
        m[struct_defn_table[i].first]=i;
    }
    //////printing the struct equivalence table//////
	vector<vector<int>>equi_table=struct_equi(struct_defn_table);
	cout<<"##############################################"<<endl;
	cout<<"Prinitng the Results (The ordering is according to number in above tables)"<<endl;
	cout<<endl<<"Printing the struct equivalence table: \n";
	print_equivalence_table(equi_table);
	cout<<"##############################################"<<endl;

    //////creating function equivalence table////////
    vector<vector<int>> func_equi_table=func_equi(func_defn_table,equi_table,m);
    cout<<"Printing the function equivalence table:\n";
    print_equivalence_table(func_equi_table);
	cout<<"##############################################"<<endl;

    ////////printing the name equivalence table/////
	vector<vector<int>>name_equi_table=name_equi(symbol_table);
	cout<<"Printing the name equivalence table: \n";
	print_equivalence_table(name_equi_table);
	cout<<"Pairs which are name equivalent"<<endl;
	print_equivalence(name_equi_table,symbol_table);

    ///////////printing the internal equivalence table////////
    vector<vector<int>> internal_equi=internal(symbol_table,m,equi_table);
    cout<<"Printing internal equivalence table:\n";
	int n2=internal_equi.size();
	for(int i=0;i<n2;i++){
		for(int j=0;j<n2;j++){
			if(name_equi_table[i][j]==1){
				internal_equi[i][j]=1;
			}
		}
	}
    print_equivalence_table(internal_equi);
	cout<<"Pairs which are internal name equivalent"<<endl;
	print_equivalence(internal_equi,symbol_table);

    /////STRUCTURE EQUIVALENCE////
    vector<vector<int>> stru_equi_table(name_equi_table);
    int n=stru_equi_table.size();
    for(int i=0;i<n;i++)
    {
        stru_equi_table[i][i]=1;
    }
    struc_equivalence(stru_equi_table,symbol_table,m,equi_table);
    cout<<"printing structure equivalence table: \n";
	print_equivalence_table(stru_equi_table);
	cout<<"Pairs which are structurally equivalent"<<endl;
	print_equivalence(stru_equi_table,symbol_table);

	return 0;
}
