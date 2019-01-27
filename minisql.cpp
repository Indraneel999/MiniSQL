#include<iostream>
#include<ctype.h>
#include<bits/stdc++.h>

using namespace std;

bool is_file_exist(string fileName)		// check if a file exist
{
    std::ifstream infile(fileName);
    return infile.good();
}

bool icmp(char *a, char *b)				// string insensetive compare
{
	if(strlen(a) != strlen(b))
		return false;

	for (int i =0 ; i<strlen(a);i++)
	{
		if (toupper(a[i]) != toupper(b[i]))
			return false;
		
	}

	return true;
}

vector<string> find_tables(int argc,char *argv[])
{
	vector<string> tab_name;
	string table,temp;
	int flag =0;
	for(int i =2 ; i<argc;i++)
	{
		if (flag)
		{
			temp = argv[i];
			if(temp == ",")
			{
				continue;
			}		
			if(temp == "where")
			{
				break;
			}	
			for(int j = 0; j< temp.size();j++)
			{
				if(temp[j] == ',' && j+1 == temp.size())
				{
					tab_name.push_back(table);		
					table.clear();
				}
				else if(temp[j] == ',' &&  j == 0)
				{
					continue;
				}
				else if(temp[j] == ',')
				{
					tab_name.push_back(table);	
					table.clear();
				}
				else if(j+1 == temp.size())
				{

					table.push_back(temp[j]);
					tab_name.push_back(table);
					table.clear();
				
				}
				else
				{
					table.push_back(temp[j]);
				}

			}
			
		}
		else
		{
			if(icmp(argv[i],"from"))
			{
				flag = 1;
			}
		}
	}

	return tab_name;	
}

void read_metadata(map <string,int> &table_id,map <string,string> &attr,vector<vector<string> > &table_attr)
{
	int flag = 0,xflag, tab_id;
	fstream md;
	md.open("metadata.txt");

	string line,tab_name;

	while(md>>line)
	{
		if(line == "<begin_table>" && flag == 0)
		{
			flag = 1;
		}
		else if(line == "<end_table>" && (flag == 1 || flag == 2))
		{
			flag = 0;
		}
		else if(flag == 1)
		{
			tab_name = line;

			cout<<"table = "<<line<<"\n";
			
			if (table_id.find(tab_name) == table_id.end()) 
			{
        		xflag = 0; 
			}
			else
			{
				xflag = 1;
				tab_id = table_id.find(tab_name)->second;
				cout<<"id = "<<tab_id<<"\n";
			}

			flag = 2;
		}
		else if(xflag == 1)
		{
			cout<<line<<"\n";
			
			table_attr[tab_id].push_back(line);
			
			std::map<string, string>::iterator it = attr.find(line); 
			if (it != attr.end()) 
			{
				 it->second = "0";				 
			}
			else
			{
				attr.insert({line,tab_name});
			}


			

		}
		
	}


}


int main(int argc, char *argv[])
{
	map <string,int> table_id;
	map <string,string> attr;
	vector<vector<string> > table_attr(64);
	char command[20];
	strcpy(command,argv[1]);
	int no_of_tables;
	vector<string> tab_name;
	fstream file[64];
		
	string temp; 				// delete

	if (icmp("select", command))
	{
	
		tab_name = find_tables(argc,argv);

		for(int i = 0; i<tab_name.size();i++)
		{
			string fname;
			fname = tab_name[i] + ".csv";		
			if(!is_file_exist(fname))
			{
				cout<<"Table does not exist";
				return 0;
			}
			file[i].open(fname);

			if (table_id.find(tab_name[i]) != table_id.end()) 
			{
				cout<<"Ambiguous table name";
        		return 0; 
			}


			table_id.insert({tab_name[i],i});

		}

		if(tab_name.size() > 64)
		{
			cout<<"Too many tables (Max limit = 64)";
		}

		read_metadata(table_id,attr,table_attr);
		
		cout<<table_attr[1][2];
		




	}
	else
	{
		cout<<"Undefined Querry(No Select)";
	}
	cout<<"\n";
	return 0;
}