#include<fstream.h>
#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<iomanip.h>

#define NOT_FOUND -1

const int false= 0;
const int true= 1;

struct ItemData
{
	char name[20];
	double cost;
	int stock;
	int code;
	int dailyUsageRate;
	int reorderLevel;
};

class Item
{
private:
	int m_quantity;
	int m_index;

public:
	static ItemData s_itemData[30];
	static int s_itemCount;

	Item()
		: m_quantity( 0), m_index( 0)
	{
	}

	void setQuantity( const int quantity)
	{
		if( m_quantity< quantity)
			s_itemData[m_index].stock-= quantity- m_quantity;

		else if( m_quantity> quantity)
			s_itemData[m_index].stock+= m_quantity- quantity;

		m_quantity= quantity;
	}

	static void calculateReorderLevel( const int index) const
	{
		const int leadTime= 2;
		const int safetyStock= 10;

		ItemData &itemData= s_itemData[index];
		itemData.reorderLevel= ( itemData.dailyUsageRate* leadTime)+ safetyStock;
	}

	void setIndex( const int index){ m_index= index;}
	const char* getItemName() const{	return s_itemData[m_index].name;}
	const double getItemCost() const{	return s_itemData[m_index].cost;}
	const int getItemStock() const {	return s_itemData[m_index].stock;}
	const int getCode() const{  return s_itemData[m_index].code; }
	const int getQuantity() const{  return m_quantity; }
	const double getItemTotal() const { return m_quantity* getItemCost();}
};
ItemData Item:: s_itemData[30];
int Item:: s_itemCount;

class CashRegister
{
private:
	double m_total;

public:
	CashRegister()
		: m_total( 0)
	{
	}

	double getTotal() const{	return m_total;}
	void deleteAmount( const double amount){    m_total-= amount; }
	void scan( const Item item){	m_total+= item.getItemTotal();}
} cashRegister;

inline void displayMessage( char string[])
{
	cout<<'\n'<<string<<'\n';
	getch();
}

int searchItemData( const int code)
{
	for( int index= 0; index< Item:: s_itemCount; ++index)
		if( code== Item:: s_itemData[index].code)
			return index;

	displayMessage( "Not found");
	return NOT_FOUND;
}

void setCode( int &code)
{
	int itemCount= Item:: s_itemCount;

	if( itemCount== 0)
		code= 1;

	else
		code= Item:: s_itemData[itemCount- 1].code+ 1;
}

void addItemData()
{
	ItemData &itemData= Item:: s_itemData[Item:: s_itemCount];

	cout<<"\nName  : ";
	gets( itemData.name);

	cout<<"Cost  : ";
	cin>>itemData.cost;

	cout<<"Stock : ";
	cin>>itemData.stock;

	cout<<"Maximum Daily Usage Rate: ";
	cin>>itemData.dailyUsageRate;

	setCode( itemData.code);
	++Item:: s_itemCount;
}

int getCodeFromUser()
{
	int searchCode;
	cout<<"\nCode: ";
	cin>>searchCode;

	return searchCode;
}

void deleteItemData()
{
	const int index= searchItemData( getCodeFromUser());
	if( index== NOT_FOUND)
		return;

	for( int j= index; j!= Item:: s_itemCount; ++j)
		Item:: s_itemData[j]= Item:: s_itemData[j+1];

	displayMessage(  "Item successfully deleted");
	--Item:: s_itemCount;
}

int getChoice( int a, int b)
{
	int choice;
	do
	{
		cout<<"Enter your choice: ";
		cin>>choice;
	}	while(choice<a|| choice>b);

	return choice;
}

void modifyItemData()
{
	const int index= searchItemData( getCodeFromUser());
	if( index== NOT_FOUND)
		return;

	ItemData &itemData= Item:: s_itemData[index];

	cout<<"\n\n1. Name : "<<itemData.name
		 <<"\n2. Cost : "<<itemData.cost
		 <<"\n3. Stock: "<<itemData.stock
		 <<"\n4. Maximum Daily Usage Rate: "<<itemData.dailyUsageRate
		 <<"\n5. Cancel\n\n";

	switch( getChoice( 1, 5))
	{
	case 1:
		cout<<"\nName: ";
		gets( itemData.name);
		break;

	case 2:
		cout<<"\nCost: ";
		cin>>itemData.cost;
		break;

	case 3:
		cout<<"\nStock: ";
		cin>>itemData.stock;
		break;

	case 4:
		cout<<"\nMaximum Daily Usage Rate: ";
		cin>>itemData.dailyUsageRate;
		break;
	}
}

inline void displayItemData( ItemData itemData)
{
	cout<<' '<<setw(7)<<itemData.code
		 <<setw(40)<<itemData.name
		 <<setw(10)<<itemData.cost
		 <<setw(8)<<itemData.stock
		 <<itemData.reorderLevel<<'\n';
}

inline void drawLine()
{
	cout<<"\n----------------------------------------"
		 <<"----------------------------------------\n";
}

inline void displayItemHeader()
{
	drawLine();
	cout<<setw(8)<<"Code"<<setw(40)<<"Item"<<setw(10)<<"Price"
		 <<setw(8)<<"Stock"<<"ROL\n\n";
}

void displayItemMenu()
{
	clrscr();
	cout.setf( ios:: left);
	displayItemHeader();

	for( int i= 0, no= Item:: s_itemCount; i< no; ++i)
		displayItemData( Item:: s_itemData[i]);

	getch();
}

void initialiseItemData()
{
	ifstream fin( "item.dat", ios::binary);

	fin.read( (char *)( Item:: s_itemData), sizeof( Item:: s_itemData));
	Item:: s_itemCount= fin.tellg()/ sizeof( ItemData);

	fin.close();
}

int searchItem( const int code)
{
	ifstream fin( "shoppingCart.dat", ios::binary);
	Item item;
	int index= 0;

	while( fin.read( (char *)&item, sizeof( Item)))
	{
		if(item.getCode()== code)
		{
			fin.close();
			return index;
		}

		else
			++index;
	}
	fin.close();

	return NOT_FOUND;
}

void clear( int x, int y)
{
	gotoxy( x, y);
	for( int i= 0; i< 12; ++i)
		cout<<"                                                             \n";
	gotoxy( x, y);
}

void displayBill()
{
	drawLine();

	cout.setf( ios:: left);
	cout<<setw(8)<<"Code"<<setw(20) <<"Item"
		 <<setw(19)<<"Quantity"<<setw(10)<<"Total Price\n";

	ifstream fin( "shoppingCart.dat", ios:: binary);
	Item item;

	while( fin.read( (char *)&item, sizeof(item)))
	{
		cout<<setw(8)<<item.getCode()
			 <<setw(20)<<item.getItemName()
			 <<setw(19)<<item.getQuantity()
			 <<setw(10)<<item.getItemTotal()<<'\n';
	}

	drawLine();
	fin.close();
}

int getQuantityFromUser( Item item)
{
	int quantity;

	do
	{
		cout<<"\nQuantity: ";
		cin>>quantity;

		if( quantity> item.getItemStock())
			cout<<"\nOut of stock!\n";

	}	while( quantity> item.getItemStock()|| quantity<= 0);

	return quantity;
}

Item getItem( const int index)
{
	ifstream fin( "shoppingCart.dat", ios:: binary);
	Item item;

	fin.seekg( index* sizeof( Item));
	fin.read( (char *)&item, sizeof( item));
	fin.close();

	return item;
}

void replaceItem( const int index, const Item item)
{
	fstream file( "shoppingCart.dat", ios:: in| ios:: out| ios:: binary);
	file.seekp( index* sizeof( item));
	file.write( (char *)&item, sizeof( item));
	file.close();
}

void mergeDuplicates( Item &item, const int index)
{
	fstream file( "shoppingCart.dat", ios:: binary| ios:: in| ios:: out);
	Item mergedItem= getItem( index);

	mergedItem.setQuantity( mergedItem.getQuantity()+ item.getQuantity());

	replaceItem( index, mergedItem);
	item.setQuantity( 0);

	file.close();
}

void insertItem()
{
	const int index= searchItemData( getCodeFromUser());
	if( index== NOT_FOUND)
		return;

	Item item;

	item.setIndex( index);
	item.setQuantity( getQuantityFromUser( item));
	cashRegister.scan( item);

	int code= item.getCode();
	if( searchItem( code)== NOT_FOUND)
	{
		ofstream fout( "shoppingCart.dat", ios::binary| ios::app);
		fout.write( (char *)&item, sizeof(item));
		fout.close();
	}
	else
		mergeDuplicates( item, index);
}

void deleteItem()
{
	const int index= searchItem( getCodeFromUser());
	if( index== NOT_FOUND)
	{
		displayMessage( "Not Found");
		return;
	}

	ifstream fin( "shoppingCart.dat", ios:: binary);
	ofstream fout( "temp.dat", ios:: binary);
	Item item;
	int position= 0;

	while( fin.read( (char *)&item, sizeof( item)))
	{
		if( position!= index)
			fout.write( ( char *)&item, sizeof( item));

		else
		{
			cashRegister.deleteAmount( item.getItemTotal());
			item.setQuantity( 0);
			displayMessage( "Item successfully deleted");
		}

		++position;
	}

	fin.close();
	fout.close();

	remove( "shoppingCart.dat");
	rename( "temp.dat", "shoppingCart.dat");
}

void modifyQuantity()
{
	const int index= searchItem( getCodeFromUser());
	if( index== NOT_FOUND)
	{
		displayMessage( "Not Found");
		return;
	}

	Item item= getItem( index);
	cashRegister.deleteAmount( item.getItemTotal());

	item.setQuantity( getQuantityFromUser( item));
	cashRegister.scan( item);

	replaceItem( index, item);
}

inline void displayItemOptions()
{
	cout<<"\n1. Insert"
		 <<"\n2. Delete"
		 <<"\n3. Modify Quantity"
		 <<"\n4. Display Item Data"
		 <<"\n5. Exit\n\n";
}

void saveItemData()
{
	ofstream fout( "item.dat", ios:: binary);

	for( int i= 0; i< Item:: s_itemCount; ++i)
	{
		ItemData itemData= Item:: s_itemData[i];
		fout.write( (char *)&itemData, sizeof( ItemData));
	}
	fout.close();
}

inline void clearFile( char fileName[])
{
	ofstream fout( fileName);
	fout.close();
}

void processItemOptions( int choice)
{
	switch( choice)
	{
	case 1:
		insertItem();
		break;

	case 2:
		deleteItem();
		break;

	case 3:
		modifyQuantity();
		break;

	case 4:
		displayItemMenu();
		break;

	case 5:
		displayMessage( "Thank you for purchasing our products!");
		saveItemData();
		clearFile( "shoppingCart.dat");
		break;
	}
}

class Admin
{
private:
	char m_username[30];
	char m_password[30];
	int m_loggedIn;

public:
	Admin(char name[30]="", char password[30]="")
	{
		strcpy( m_username, name);
		strcpy( m_password, password);
		m_loggedIn= false;
	}

	char* getUsername(){		return m_username;	}
	char* getPassword(){		return m_password;  }
	int hasLoggedIn()  {		return m_loggedIn;  }
	void setLogIn( int bool){	m_loggedIn= bool;   }
} admin;

char* getUsernameFromUser()
{
	char username[30];
	cout<<"\nUsername: ";
	gets( username);

	return username;
}

char* getPasswordFromUser()
{
	const int backSpace= 8;
	const int enter= 13;
	char password[30];
	char character;
	int i= 0;

	cout<<"Password: ";
	do
	{
		character= getch();

		if( character== backSpace)
		{
			if( i!= 0)
			{
				cout<<"\b \b";
				--i;
			}
		}

		else if( character!= enter)
		{
			cout<<'*';
			password[i++]= character;
		}
	}	while( character!= enter);

	password[++i]= '\0';

	return password;
}

void logIn()
{
	clrscr();

	char username[30];
	char password[30];

	strcpy( username, getUsernameFromUser());
	strcpy( password, getPasswordFromUser());

	if( !strcmpi( password, admin.getPassword()) &&
		 !strcmpi( username, admin.getUsername()))
	{
		displayMessage( "\nLog in successful");
		admin.setLogIn( true);
	}

	else
		displayMessage( "\nIncorrect username or password");
}

void signUp()
{
	clrscr();

	char username[30];
	char password[30];

	strcpy( username, getUsernameFromUser());
	strcpy( password, getPasswordFromUser());

	admin= Admin( username, password);
}

void mainMenu()
{
	int choice;
	do
	{
		clrscr();
		cout<<"\nMain Menu\n\n\n\n"
			 <<"1) Sign up\n"
			 <<"2) Log in\n"
			 <<"3) Exit\n\n";

		switch( getChoice(1, 3))
		{
		case 1:
			signUp();
			break;

		case 2:
			logIn();
			break;

		case 3:
			return;
		}
	}	while( !admin.hasLoggedIn());
}

void checkReorderLevel()
{
	int found= 0;

	for( int i= 0; i< Item:: s_itemCount; ++i)
	{
		Item:: calculateReorderLevel( i);
		ItemData &itemData= Item:: s_itemData[i];

		if( itemData.stock<= itemData.reorderLevel)
		{
			cout<<"\nReorder Level of "<<itemData.name
				 <<" (Code: "<<itemData.code<<") "
				 <<" has been reached.";

			found= 1;
		}
	}

	if( found)
		cout<<"\nPlease replenish supplies later. \n\n";
}

void billing()
{
	int choice;
	do
	{
		clrscr();
		displayBill();

		cout<<"Your bill is Rs "<<cashRegister.getTotal()<<'\n';
		int x= wherex();
		int y= wherey();

		displayItemOptions();
		checkReorderLevel();

		choice= getChoice( 1,5);
		clear( x, y);
		processItemOptions( choice);
	}	while( choice!= 5);
}

inline void displayAdminMenu()
{
	clrscr();
	cout<<"\nAdmin Menu\n"
		 <<"\n1. Add item"
		 <<"\n2. Delete item"
		 <<"\n3. Search item"
		 <<"\n4. Modify item"
		 <<"\n5. Show"
		 <<"\n6. Start Billing\n\n";
}

int getItemCountFromUser()
{
	int no;
	cout<<"\nEnter the no of items: ";
	cin>>no;

	return no;
}

void processAdminOptions( int choice)
{
	switch( choice)
	{
	case 1:
		for( int i= 0, no=  getItemCountFromUser(); i< no; ++i)
			addItemData();

		break;

	case 2:
		deleteItemData();
		break;

	case 3:
		const int index= searchItemData( getCodeFromUser());
		if( index== NOT_FOUND)
			break;

		displayItemHeader();
		displayItemData( Item:: s_itemData[index] );
		getch();

		break;

	case 4:
		modifyItemData();
		break;

	case 5:
		displayItemMenu();
		break;

	case 6:
		billing();
		return;
	}
}

void adminMenu()
{
	initialiseItemData();

	int choice;
	do
	{
		displayAdminMenu();
		checkReorderLevel();

		choice= getChoice( 1, 6);
		processAdminOptions( choice);
	}	while( choice!= 6);
}

void main()
{
	mainMenu();
	if( !admin.hasLoggedIn())
		return;

	adminMenu();
}

