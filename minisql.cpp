#include<iostream>
#include<ctype.h>
#include<bits/stdc++.h>

using namespace std;

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

vector<string> find_num_of_tables(int argc,char *argv[])
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
int main(int argc, char *argv[])
{
	char command[20];
	strcpy(command,argv[1]);
	cout<<"argc = "<<argc<<"\n";
	int no_of_tables;
	vector<string> tab_name;



	if (icmp("select", command))
	{
	
		tab_name = find_num_of_tables(argc,argv);

		for(int i = 0; i<tab_name.size();i++)
		{
			cout<<i<<".  "<<tab_name[i]<<"\n";
		}










	}
	else
	{
		cout<<"Undefined Querry(No Select)";
	}
	cout<<"\n";
	return 0;
}