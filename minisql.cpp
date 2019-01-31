#include<iostream>
#include<ctype.h>
#include<bits/stdc++.h>

using namespace std;

int total_cols = 0;

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

vector<string> find_tables(int argc,char *argv[],int f = 1)
{
	vector<string> tab_name;
	string table,temp;
	int flag =0;
	for(int i =1 ; i<argc;i++)
	{
		if (flag)
		{
			temp = argv[i];
			if(temp == ",")
			{
				continue;
			}		
			if(temp == "where" && f == 1)
			{
				break;
			}	
			if(temp == "from" && f == 2)
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
			if(icmp(argv[i],"from")== true && f == 1)
			{
				flag = 1;
			}
			if(icmp(argv[i],"select")== true && f == 2)
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
			total_cols++;
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
vector<string> divide_row(string row)
{
	vector<string> row_elem;
	string temp;

	for(int i=0;i<row.size();i++)
	{
		if(row[i] == ',')
		{
			row_elem.push_back(temp);
			temp.erase();
		}
		else
		{
			temp.push_back(row[i]);
		}
	}
	row_elem.push_back(temp);
	return row_elem;
}

string extract_col(string col_name, string tab_name)
{
	if(col_name == "*")
	{
		return col_name;
	}

	string tab[2];
	int k = 0;

	for(int i = 0;i<col_name.size();i++)
	{
		if(col_name[i] == '.')
		{
			k = 1;
			continue;
		}

		tab[k].push_back(col_name[i]);
	}

	if(k == 0)
	{
		return tab[0];
	}
	else
	{
		// cout<<tab[0]<<" "<<tab[1]<<" "<<tab_name<<"\n";
		if(tab[0] == tab_name)
		{
			return tab[1] + "*";

		}
		else
		{
			return "**";
		}
	}

	 
}

vector<vector<string>> output;
int join_print_tables(vector<string> &tab_name,vector<string> &col_name,vector<string> &line,map <string,string> &attr,vector<vector<string> > &table_attr, int i, int n)
{
	if(i==n)
	{
		// for(int j= 0; j< line.size();j++)
		// 	cout<<line[j]<<" : ";
		output.push_back(line);
		return 0;

	}
	fstream file;
	string fname,row,print_col;

	vector<string> row_elem;


	fname = tab_name[i] + ".csv";	
	file.open(fname);
	while(file>>row)
	{
		int size_count = 0;
		row_elem = divide_row(row);

		for(int x = 0; x < col_name.size() ; x++)
		{
			print_col = extract_col(col_name[x],tab_name[i]);
			if(print_col == "*")
			{
				line.insert(line.end(), row_elem.begin(), row_elem.end());
				size_count += row_elem.size();
			}
			else if(print_col == "**")
			{
				continue;
			}
			else if(print_col[print_col.size() - 1] == '*')
			{
				print_col.pop_back();
				int y,flag = 0;
				for(y =0 ; y< table_attr[i].size();y++)
				{
					if(table_attr[i][y] == print_col)
					{
						flag  = 1;
						break;
					}
				}
				if(flag == 0)
				{
					cout<<"Error : attribute not found";
					return -1;
				}
				else
				{
					line.push_back(row_elem[y]);
					size_count++;	
				}

			}
			else
			{
				std::map<string, string>::iterator it = attr.find(print_col); 
				if (it != attr.end()) 
				{
					if(it->second == tab_name[i])
					{
						int y,flag = 0;
						for(y =0 ; y< table_attr[i].size();y++)
						{
							if(table_attr[i][y] == print_col)
							{
								flag  = 1;
								break;
							}
						}
						if(flag == 0)
						{
							cout<<"Error : attribute not found";
							return -1;
						}
						else
						{
							line.push_back(row_elem[y]);
							size_count++;	
						}
					}
				}
				else
				{
					cout<<"Error : attribute not found";
					return -1;
				}

			}	

		}


		if(join_print_tables(tab_name,col_name,line,attr,table_attr,i+1,n) == -1)
		{
			return -1;
		}
		for(int k = 0 ; k< size_count	;k++)
		{
			line.pop_back();
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	map <string,int> table_id;				// mapping table to a table id
	map <string,string> attr;				// mapping attribute to the table it belongs
	vector<vector<string> > table_attr(64);	// attribute list for each table stored according to the table id
	char command[20];
	strcpy(command,argv[1]);
	int no_of_tables;
	vector<string> tab_name;				//list of all tables
	vector<string> col_name;				//list of columns to print


	fstream file[64];
		
	string temp; 							// delete

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
		
		cout<<"total_cols = "<<total_cols<<"\n";

		col_name = find_tables(argc,argv,2);
		cout<<"col_name = ";




		for(int i = 0; i < col_name.size();i++)
		{
			cout<<col_name[i]<<":";
		}
		cout<<"\n";


		vector<string> line;
		join_print_tables(tab_name,col_name,line,attr,table_attr,0,tab_name.size());

		for(int i = 0,j; i < output.size() ; i++)
		{
			for(j = 0 ; j < output[i].size()-1;j++)
			{
				cout<<output[i][j]<<":";
			}
			cout<<output[i][j]<<"\n";
		}


	}
	else
	{
		cout<<"Undefined Querry(No Select)";
	}
	cout<<"\n";
	return 0;
}