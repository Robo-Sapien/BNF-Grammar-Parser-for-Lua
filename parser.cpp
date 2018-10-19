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

	cout<<"Creating the symbol Table"<<endl;
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

	}
}



///////////////// MAIN FUNCTION ////////////////////////////
int main(int argc, char** argv){
	string code="erferf int a,b[100],c5; float c,d,f;  abhinabddefvefv int a1,a2,a3; int abiefvjef;";
	string code2="bbb int* i,*j [15];struct node{int a;float b;};struct node* *n1,n2  [200];";

	//Regex-Patterns for interger and float parsing
	regex int_pattern("int.*?;");
	regex float_pattern("float.*?;");
	regex struct_defn_pattern("struct.*[{].*?[}];");
	regex struct_dec_pattern("struct.*?;");

	//Parsing the declaration statements form the whole code
	vector<string> parsed_stmts;//this will hold the statements
	vector<string> parsed_defn;//this will hold the definitions

	declaration_parser(code2,struct_defn_pattern,parsed_defn);
	declaration_parser(code2,struct_dec_pattern,parsed_stmts);
	declaration_parser(code2,int_pattern,parsed_stmts);
	declaration_parser(code2,float_pattern,parsed_stmts);

	for(unsigned int i=0;i<parsed_stmts.size();i++){
		cout<<parsed_stmts[i]<<endl;
	}

	//Making the symbol table from the parsed statements
	vector<tuple<string,string,int,int>> symbol_table;
	fill_symbol_table(parsed_stmts,symbol_table);

	return 0;
}
