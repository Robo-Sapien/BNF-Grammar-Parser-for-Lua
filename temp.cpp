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

	cout<<endl<<"Creating the symbol Table"<<endl;
	//Defining the regex pattern for parsing dtype
	smatch match_itr;	//the iterator for the matching pos
	regex pointer_pattern("(int|float|struct[\\s]+[\\S]+)[*]");
	regex normal_pattern("(int|float|struct[\\s]+[\\S]+)");

	//Starting the interation one by one over all declaration stmts
	for(unsigned int i=0;i<parsed_stmts.size();i++){
		string stmt=parsed_stmts[i];
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
		cout<<stmt<<endl;

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
			cout<<var_type<<" "<<var_name<<" "<<var_pointer_count<<" "<<arr_size<<endl;
			//Saving the varaible in the symbol tuple
			symbol_table.push_back(make_tuple(var_type,var_name,var_pointer_count,arr_size));
			//Restoring for next variable
			var_name="";
			var_pointer_count=pointer_count;
			arr_size=0;
		}
		cout<<endl;
	}
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
    cout<<get<0>(tup1[0])<<endl;
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
		cout<<"Parserd declaration statements"<<endl;
		for(unsigned int i=0;i<dec_stmts_vector.size();i++){
			cout<<dec_stmts_vector[i]<<endl;
		}

		//Now creating the symbol table for the curretn definition
		vector<tuple<string,string,int,int>> defn_table;
		fill_symbol_table(dec_stmts_vector,defn_table);
		struct_defn_table.push_back(make_pair(snames,defn_table));
	}
	return struct_defn_table;
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
///////////////// MAIN FUNCTION ////////////////////////////
int main(int argc, char** argv){
	string code="erferf int a,b[100],c5; float c,d,f;  abhinabddefvefv int a1,a2,a3; int abiefvjef;";
	string code2="	bbb int i,j,*j[15],*k[15];\
					struct node{int a;float b;struct node c1;};\
					struct node1{int a;float b;struct node c2;};\
					struct node* *n1,*n2,n3  [200];\
					struct node1* *n4;\
					int func1(int a,float b,struct node cq){blah;blah;return balh};\
					int func2(int a,int b,struct node1 c1){};\
					";

	//Regex-Patterns for interger and float parsing
	regex int_pattern("int.*?;");
	regex float_pattern("float.*?;");
	regex struct_defn_pattern("struct.*?[{].*?[}];");
	regex struct_dec_pattern("struct.*?;");
	//Regex-Pattern for function extraction
	regex func_dec_pattern("(?:int|float|struct).*[(].*?[)];")

	//Parsing the declaration statements form the whole code
	vector<string> parsed_stmts;//this will hold the statements
	vector<string> parsed_defn;//this will hold the definitions

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

	//Making the symbol table from the parsed statements
	cout<<"##############################################"<<endl;
	vector<tuple<string,string,int,int>> symbol_table;
	fill_symbol_table(parsed_stmts,symbol_table);
	//Priingng the symbol table
	cout<<endl<<"Prinitng the symbol table"<<endl;
	for(unsigned int i=0;i<symbol_table.size();i++){
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
		cout<<"Printing the struct definition for: "<<struct_defn_table[i].first<<endl;
		vector<tuple<string,string,int,int>> struct_symbol_table;
		struct_symbol_table=struct_defn_table[i].second;
		for(unsigned int j=0;j<struct_symbol_table.size();j++){
			cout<<get<0>(struct_symbol_table[j])<<"\t\t";
			cout<<get<1>(struct_symbol_table[j])<<"\t\t";
			cout<<get<2>(struct_symbol_table[j])<<"\t\t";
			cout<<get<3>(struct_symbol_table[j])<<"\t\t";
			cout<<endl;
		}
	}
	//////printing the struct equivalence table//////
	int i,j,n;
	vector<vector<int>>equi_table=struct_equi(struct_defn_table);
	n=equi_table.size();

	cout<<endl<<"Printing the struct equivalence table: \n";
	for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            cout<<equi_table[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;

    ////////printing the name equivalence table/////
	vector<vector<int>>name_equi_table=name_equi(symbol_table);
	n=name_equi_table.size();

	cout<<"Printing the name equivalence table: \n";
	for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            cout<<name_equi_table[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;

    /////STRUCTURE EQUIVALENCE////
    vector<vector<int>> stru_equi_table(name_equi_table);
    n=stru_equi_table.size();
    for(i=0;i<n;i++)
    {
        stru_equi_table[i][i]=1;
    }
    map<string,int> m;
    for(i=0;i<struct_defn_table.size();i++)
    {
        m[struct_defn_table[i].first]=i;
    }
    struc_equivalence(stru_equi_table,symbol_table,m,equi_table);
    cout<<"printing structure equivalence table: \n";
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            cout<<stru_equi_table[i][j]<<" ";
        }
        cout<<endl;
    }
	return 0;
}
