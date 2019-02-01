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
			if(temp == "distinct" && f == 2)
			{
				continue;
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
				 it->second = "**";				 
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

string extract_col(string col_name, string &tab_name)
{
	if(col_name == "*")
	{
		return col_name;
	}

	string tab[2];
	int k = 0;
	tab[0] = "";
	tab[1] = "";
	tab_name = "**";

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
		tab_name = tab[0];
		return tab[1];
	}

	 
}

vector<vector<string>> output;
vector<vector<string>> final_output(1024);
int join_print_tables(vector<string> &tab_name,vector<string> &col_name,vector<string> &line,map <string,string> &attr,vector<vector<string> > &table_attr, int i, int n)
{
	if(i==n)
	{
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

		line.insert(line.end(), row_elem.begin(), row_elem.end());
		size_count = row_elem.size();

		if(join_print_tables(tab_name,col_name,line,attr,table_attr,i+1,n) == -1)
		{
			return -1;
		}
		for(int k = 0 ; k< size_count ; k++)
		{
			line.pop_back();
		}
	}

	return 0;
}
string check_condition(string cond,string (&cond_var)[2])
{
	string cond_op;
	int j = 0;

	for(int i =0 ; i<cond.size() ; i++)
	{
		if(cond[i] == '=')
		{
			cond_op = '=';
			j++;
		}
		else if(cond[i] == '>')
		{
			cond_op = '>';
			j++;
			if(cond[i+1] == '=')
			{
				cond_op = ">=";
				i++;
			}
		}
		else if(cond[i] == '<')
		{
			cond_op = '<';
			j++;
			if(cond[i+1] == '=')
			{
				cond_op = "<=";
				i++;
			}
		}
		else
		{
			cond_var[j].push_back(cond[i]);
		}
	}

	if(j>1)
	{
		return "**";
	}
	else
	{
		return cond_op;
	}

}
bool check_condition(int a,int b,string cond_op)
{
	if(cond_op == "=")
	{
		if(a==b)
			return true;
		else 
			return false;
	}
	if(cond_op == "<")
	{
		if(a<b)
			return true;
		else 
			return false;
	}
	if(cond_op == ">")
	{
		if(a>b)
			return true;
		else 
			return false;
	}
	if(cond_op == "<=")
	{
		if(a<=b)
			return true;
		else 
			return false;
	}
	if(cond_op == ">=")
	{
		if(a>=b)
			return true;
		else 
			return false;
	}


}

int final_print(vector<string> &tab_name,vector<string> &col_name,map <string,string> &attr,vector<vector<string> > &table_attr, vector<string> table_head , string cond_type, string (&cond)[2])
{
	string print_col;
	for(int i = 0 ; i < output.size() ; i++)
	{
		string cond_op,cond_var[2],restricted[2];
		if(cond_type == "*")
		{
			cout<<"No Condition";
		}
		else
		{
			// cout<<"One Condition";
			long val[2];
			cond_op = check_condition(cond[0],cond_var);
			if(cond_op == "**")
			{
				cout<<"Error : Too many operators";
				return 0;
			}
			else
			{
				// cout<<cond_op<<" "<<cond_var[0]<<" "<<cond_var[1]<<"\n";
				char *p,*q;
				
				int fl = 0;
				val[0] = strtol(cond_var[0].c_str(), &p, 10);
				if (*p)
				{
					string find_col, tb;
					fl = 1;
					find_col = extract_col(cond_var[0],tb);

					if(tb != "**")
					{
						std::map<string, string>::iterator it = attr.find(find_col); 
						if (it != attr.end()) 
						{
							int j;
							for(j = 0 ; j < table_head.size() ; j++)
							{
								if(table_head[j] == cond_var[0])
								{
									// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
									val[0] = strtol(output[i][j].c_str(), &q, 10);
									if (*q)
									{
										cout<<"Data type error :";
										return 0;
									}
									
									break;
								}
							}

							if( j == table_head.size())
							{
								cout<<"Column name error ";
								return 0;
							}
						}
						else
						{
							cout<<"Column name error ";
							return 0;
						}
					}
					else
					{
						std::map<string, string>::iterator it = attr.find(find_col); 
						if (it != attr.end()) 
						{
							if(it->second == "**")
							{
								cout<<"Ambiguous column name ";
								return 0;
							} 	
							else
							{
								find_col = it->second + "." + find_col;
								int j;
								for(j = 0 ; j < table_head.size() ; j++)
								{
									if(table_head[j] == find_col)
									{
										// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
										val[0] = strtol(output[i][j].c_str(), &q, 10);
										if (*q)
										{
											cout<<"Data type error :";
											return 0;
										}
										break;
									}
								}

								if( j == table_head.size())
								{
									cout<<"Column name error";
									return 0;
								}
							}
						}
						else
						{
							cout<<"Column name error";
							return 0;
						}
					}
			
						
				}
				

				val[1] = strtol(cond_var[1].c_str(), &p, 10);
				if (*p)
				{
					string find_col, tb;	
					find_col = extract_col(cond_var[1],tb);

					
					if(tb != "**")
					{
						std::map<string, string>::iterator it = attr.find(find_col); 
						if (it != attr.end()) 
						{
							int j;
							for(j = 0 ; j < table_head.size() ; j++)
							{
								if(table_head[j] == cond_var[1])
								{
									// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
									if(fl == 1)
									{
										restricted[0]=table_head[j];
									}
									val[1] = strtol(output[i][j].c_str(), &q, 10);
									if (*q)
									{
										cout<<"Data type error :";
										return 0;
									}
									break;
								}
							}

							if( j == table_head.size())
							{
								cout<<"Column name error 1";
								return 0;
							}
						}
						else
						{
							cout<<"Column name error 2";
							return 0;
						}
					}
					else
					{
								
						std::map<string, string>::iterator it = attr.find(find_col); 
						if (it != attr.end()) 
						{
							if(it->second == "**")
							{
								cout<<"Ambiguous column name ";
								return 0;
							} 	
							else
							{
								find_col = it->second + "." + find_col;
								int j;
								for(j = 0 ; j < table_head.size() ; j++)
								{
									if(table_head[j] == find_col)
									{
										// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
										if(fl == 1)
										{
											restricted[0]=table_head[j];
										}
										val[1] = strtol(output[i][j].c_str(), &q, 10);
										if (*q)
										{
											cout<<"Data type error :";
											return 0;
										}
										break;
									}
								}

								if( j == table_head.size())
								{
									cout<<"Column name error";
									return 0;
								}
							}
						}
						else
						{
							cout<<"Column name error";
							return 0;
						}
					}


			
						
				}
				

			}

			bool return_val = check_condition(val[0],val[1],cond_op);

			if(return_val == false && (cond_type=="and" || cond_type == "**"))
			{
				continue;
			}
			else if(return_val == true && (cond_type == "or" || cond_type == "**"))
			{
				// cout<<"True";
			}
			else
			{

				cond_var[0]="";
				cond_var[1]="";
				cond_op = check_condition(cond[1],cond_var);
				// cout<<"cond_op = "<<cond_var[0]<<" "<<cond_var[1];
				if(cond_op == "**")
				{
					cout<<"Error : Too many operators";
					return 0;
				}

				else
				{
					// cout<<cond_op<<" "<<cond_var[0]<<" "<<cond_var[1]<<"\n";
					char *p,*q;
					
					int fl = 0;
					val[0] = strtol(cond_var[0].c_str(), &p, 10);
					if (*p)
					{
						string find_col, tb;
						fl = 1;
						find_col = extract_col(cond_var[0],tb);
						// cout<<"find_col = "<<find_col<<"\t\t"<<cond_var[0];
						if(tb != "**")
						{
							std::map<string, string>::iterator it = attr.find(find_col); 
							if (it != attr.end()) 
							{
								int j;
								for(j = 0 ; j < table_head.size() ; j++)
								{
									if(table_head[j] == cond_var[0])
									{
										// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
										val[0] = strtol(output[i][j].c_str(), &q, 10);
										if (*q)
										{
											cout<<"Data type error :";
											return 0;
										}
										
										break;
									}
								}

								if( j == table_head.size())
								{
									cout<<"Column name error 1";
									return 0;
								}
							}
							else
							{
								cout<<"Column name error 21";
								return 0;
							}
						}
						else
						{
							std::map<string, string>::iterator it = attr.find(find_col); 
							if (it != attr.end()) 
							{
								if(it->second == "**")
								{
									cout<<"Ambiguous column name ";
									return 0;
								} 	
								else
								{
									find_col = it->second + "." + find_col;
									int j;
									for(j = 0 ; j < table_head.size() ; j++)
									{
										if(table_head[j] == find_col)
										{
											// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
											val[0] = strtol(output[i][j].c_str(), &q, 10);
											if (*q)
											{
												cout<<"Data type error :";
												return 0;
											}
											break;
										}
									}

									if( j == table_head.size())
									{
										cout<<"Column name error";
										return 0;
									}
								}
							}
							else
							{
								cout<<"Column name error";
								return 0;
							}
						}
				
							
					}
					

					val[1] = strtol(cond_var[1].c_str(), &p, 10);
					if (*p)
					{
						string find_col, tb;	
						find_col = extract_col(cond_var[1],tb);

						
						if(tb != "**")
						{
							std::map<string, string>::iterator it = attr.find(find_col); 
							if (it != attr.end()) 
							{
								int j;
								for(j = 0 ; j < table_head.size() ; j++)
								{
									if(table_head[j] == cond_var[1])
									{
										// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
										if(fl == 1)
										{
											restricted[0]=table_head[j];
										}
										val[1] = strtol(output[i][j].c_str(), &q, 10);
										if (*q)
										{
											cout<<"Data type error :";
											return 0;
										}
										break;
									}
								}

								if( j == table_head.size())
								{
									cout<<"Column name error 1";
									return 0;
								}
							}
							else
							{
								cout<<"Column name error 22";
								return 0;
							}
						}
						else
						{
									
							std::map<string, string>::iterator it = attr.find(find_col); 
							if (it != attr.end()) 
							{
								if(it->second == "**")
								{
									cout<<"Ambiguous column name ";
									return 0;
								} 	
								else
								{
									find_col = it->second + "." + find_col;
									int j;
									for(j = 0 ; j < table_head.size() ; j++)
									{
										if(table_head[j] == find_col)
										{
											// cout<<"yipieeeeeee "<<table_head[j]<<"  "<<output[i][j]<<"\t";
											if(fl == 1)
											{
												restricted[0]=table_head[j];
											}
											val[1] = strtol(output[i][j].c_str(), &q, 10);
											if (*q)
											{
												cout<<"Data type error :";
												return 0;
											}
											break;
										}
									}

									if( j == table_head.size())
									{
										cout<<"Column name error";
										return 0;
									}
								}
							}
							else
							{
								cout<<"Column name error";
								return 0;
							}
						}


				
							
					}
					

				}
				return_val = check_condition(val[0],val[1],cond_op);
				// cout<<"return_val = "<<val[0]<<"\t"<<val[1]<<"\t"<<return_val<<"\n";
				if(return_val== false)
				{
					continue;
				}
				else
				{
					// cout<<"trueeeee";
				}

			}

		}
		
		for(int x = 0; x < col_name.size() ; x++)
		{
			string tab;
			print_col = extract_col(col_name[x],tab);
			if(print_col == "*")
		 	{
		 		for(int j = 0 ; j < output[i].size() ; j++)
		 		{
		 			if(table_head[j] == restricted[0])
		 				continue;
		 			cout<<output[i][j]<<"\t";
		 			final_output[i].push_back(output[i][j]);
		 		} 
		 	}
		 	else if(tab == "**")
		 	{
		 		std::map<string, string>::iterator it = attr.find(print_col); 
				if (it != attr.end()) 
				{
					if(it->second == "**")
					{
						cout<<"Ambiguous column name ";
						return 0;
					} 	
					else
					{
						print_col = it->second + "." + print_col;
						int j;
						for(j = 0 ; j < table_head.size() ; j++)
						{
							if(table_head[j] == print_col)
							{
								cout<<output[i][j]<<"\t";
								final_output[i].push_back(output[i][j]);
								break;
							}
						}

						if( j == table_head.size())
						{
							cout<<"Column name error";
							return 0;
						}
					}
				}
				else
				{
					cout<<"Column name error";
					return 0;
				}		
		 	}
		 	else
		 	{
		 		std::map<string, string>::iterator it = attr.find(print_col); 
				if (it != attr.end()) 
				{
					int j;
					for(j = 0 ; j < table_head.size() ; j++)
					{
						if(table_head[j] == col_name[x])
						{
							cout<<output[i][j]<<"\t";
							final_output[i].push_back(output[i][j]);
							break;
						}
					}

					if( j == table_head.size())
					{
						cout<<"Column name error";
						return 0;
					}
				}
				else
				{
					cout<<"Column name error";
					return 0;
				}
		 	}
		}
		cout<<"\n";		
	}

	return 1;
}

vector<string> find_table_head(vector<string> &tab_name,vector<vector<string> > &table_attr)
{
	vector<string> table_head;
	string temp;
	for(int i = 0; i < tab_name.size() ; i++)
	{
		
		for(int j = 0 ; j < table_attr[i].size() ; j++)
		{
			temp = tab_name[i] + "." + table_attr[i][j];
			table_head.push_back(temp); 
		}
	}

	return table_head;
}

string find_cond(int argc,char *argv[],string (&cond)[2])
{
	string temp,cond_type;
	cond_type = "*";
	int flag =0;
	for(int i =1 ; i<argc;i++)
	{
		if(icmp(argv[i],"and")== true)
		{
			if(flag == 1)
			{
				flag = 2;
				cond_type = "and";
				continue;
			}
			else
			{
				cout<<"Missplaced and error \n";
				return "***";
			}

		}
		if(icmp(argv[i],"or")== true)
		{
			if(flag == 1)
			{
				flag = 2;
				cond_type = "or";
				continue;
			}
			else
			{
				cout<<"Missplaced or error \n";
				return "***";
			}
		}
		if (flag != 0)
		{
			temp = argv[i];
			cond[flag-1].append(temp);
		}
		else
		{
			if(icmp(argv[i],"where")== true)
			{
				cond_type = "**";
				flag = 1;
			}
			
		}
	}

	return cond_type;	
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
	vector<string> table_head;				//header for cross-product table

	string cond_type, cond[2];

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

		table_head = find_table_head(tab_name,table_attr);

		for(int i = 0,j; i < output.size() ; i++)
			cout<<table_head[i]<<" : ";
		
		cout<<"\n";

		cond_type = find_cond(argc,argv,cond);

		if(cond_type == "***")
			return 0;

		cout<<"cond = "<<cond_type<<" "<<cond[0]<<" "<<cond[1]<<"\n";

		int ret_val;

		ret_val = final_print(tab_name,col_name,attr,table_attr,table_head,cond_type,cond);

		if(ret_val == 0)
		{
			return 0;
		}

		cout<<"\n";

		int distinct = 0,cp = 2;

		string agg,subs;

		if(icmp(argv[2],"distinct"))
		{
			distinct = 1;
		}

		if(distinct == 1)
		{
			cp++;
		}

		if()


		
		for(int i = 0,j; i < output.size() ; i++)
		{
			int flag = 1;
			if(distinct == 1)
			{	
				for(int k = 0;k<i;k++)
				{	
					flag = 0;
					for (int l = 0;l <final_output[k].size();l++)
					{
						if(final_output[i][l] != final_output[k][l])
						{
							flag =1;
							break;
						}
					}
					if(flag == 0)
						break;
				}
			}

			if(flag == 0)
				continue;

			for(j = 0 ; j < final_output[i].size()-1;j++)
			{
				cout<<final_output[i][j]<<"\t";
			}
			cout<<final_output[i][j]<<"\n";
		}

		


	}
	else
	{
		cout<<"Undefined Querry(No Select)";
	}
	cout<<"\n";
	return 0;
}