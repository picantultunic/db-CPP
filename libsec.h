/*

o mica documentatie :
	-de fiecare data cand ai o operatie cu o persoana , punel ca si utilizat ca sa nu ti se corupa memoria

ce trebuie de adaugat:
	(facut)-verifica daca persoan este deja in sistem
	-probleme cu citire din fisier
	-verifica 
bugguri:
	(nu ar mai trebuii sa fie o problema)-daca bagi doua persoane cu acelasi nume o sa il dea pe cel mai mare cu acelasi id
	(facut)-daca persoana nu are date , programul nu stie asta 
*/

#include <fstream>
#include <stdio.h>
#include <filesystem>
#include <string>
#include <bitset>
#define inFolder "\\\\"
#define CACHEPERS 16
#pragma warning(disable : 4996)
using namespace std;
using namespace std::filesystem;



//////////
///structuri de date
//////////

struct fisiereDataBase {
	bool activ = 0;
	int nrUtilizatori=0 , lastid = INT_MAX;
	string Users, nrdeutilizatori, interactiuni, legaturi, relati , modificariPosibilitati;
};

struct UserInfo{
	string UserName;
	int UserId=-1;
};

struct decizieModificarePersoana {
	char *posibilitate = nullptr;
	decizieModificarePersoana* next = nullptr;
};

struct memFisierPersonal {
	char* memblock;
	UserInfo persoana;
	struct prieteni{
		char* nume = nullptr;
		int	opinie=-1 ,id=-1;
		prieteni* next=nullptr;
	};
	struct interactiuni {
		char* cod=nullptr;
		interactiuni* next=nullptr;
	};
	prieteni* ptrn = new prieteni;
	prieteni* startPtrn = ptrn;
	interactiuni* itr = new interactiuni;
	interactiuni* startItr = itr;

	

	void nextPrieten(){
		ptrn->next = new prieteni;
		ptrn = ptrn->next;
	}
};

//////////
///variabile globale
//////////

fisiereDataBase DB;

decizieModificarePersoana* opPos = new decizieModificarePersoana;
decizieModificarePersoana* opPosP = opPos;
int marimeOPpos = 0;

UserInfo persoana;

int nrpersinuse=0;
bitset<CACHEPERS> slotpers = { false }; bitset<CACHEPERS> inusepers = { false };
memFisierPersonal *mempers[CACHEPERS];

//////////
///pentru initializare
//////////

void DBinitializare(const string numeFisierSetari) {
	ifstream in;
	in.open(numeFisierSetari);
	if (in.fail()) {
		cout << "a esuat , vedeti daca aveti acest fisier folder :" << numeFisierSetari << endl;
		return;
	}
	//databaseul este activ
	DB.activ = 1; 
	//se preia din fisier ce fisiere sa citeasca pentru database
	getline(in, DB.Users);
	getline(in, DB.nrdeutilizatori);
	getline(in, DB.interactiuni);
	getline(in, DB.relati);
	getline(in, DB.legaturi);
	getline(in, DB.modificariPosibilitati);

	//se adauga prefixul pentru a fi usor de accesat in 
	DB.Users += inFolder;
	DB.interactiuni += inFolder;
	DB.relati += inFolder;
	DB.modificariPosibilitati += ".txt";
	cout << DB.Users << endl;
}

void dateModificariPosibilitati() {
	ifstream in(DB.modificariPosibilitati);
	string x;
	marimeOPpos = 0;
	while (getline(in,x)) {
		opPosP->posibilitate = new char[x.length()+1];
		strncpy(opPosP->posibilitate, x.c_str(),x.length());
		opPosP->posibilitate[x.length()] = '\0';
		opPosP->next = new decizieModificarePersoana;
		opPosP = opPosP->next;
		marimeOPpos++;
	}
	opPosP = opPos;
	in.close();
}





void returneazaPersoanaDateDB(string numePersoana, UserInfo& Persoana);




//////////
///functii
//////////
void afisarecache() {
	system("cls");
	bool ointrat = true;
	for (int i = 0; i < CACHEPERS; i++) {
		ointrat = false;

		if (slotpers[i] == 1) {
			cout << " slot " << i << "{" << endl;
			cout << "  numele persoanei : " << mempers[i]->persoana.UserName << endl;
			cout << "  id persoanei : " << mempers[i]->persoana.UserId << endl;
			cout << "  prietenii acestuia : ";
			mempers[i]->ptrn = mempers[i]->startPtrn;
			if (mempers[i]->ptrn->nume != nullptr)
			{

				while (mempers[i]->ptrn->next != nullptr) {
					cout << mempers[i]->ptrn->nume << ',';
					mempers[i]->ptrn = mempers[i]->ptrn->next;
				}
				cout << mempers[i]->ptrn->nume;
				mempers[i]->ptrn = mempers[i]->ptrn->next;
				cout << endl;
			}
			else
				cout << "Nan" << endl;

			cout << "  opinia fata de ei : ";
			mempers[i]->ptrn = mempers[i]->startPtrn;
			if (mempers[i]->ptrn->nume != nullptr){

				while (mempers[i]->ptrn->next != nullptr) {
					cout << mempers[i]->ptrn->opinie << ',';
					mempers[i]->ptrn = mempers[i]->ptrn->next;
				}
				cout << mempers[i]->ptrn->opinie;
				mempers[i]->ptrn = mempers[i]->ptrn->next;
				cout << endl;
			}
			else
				cout << "Nan" << endl;
			cout << "}" << endl << endl << endl;
		}
	}
	if (ointrat)
		cout << "nu o intrat" << endl;

	cin.get();
}

int slotFreePersonCache() {
	for (int i = 0; i < CACHEPERS; i++) {
		if(slotpers[i]==0){
			inusepers[i] = 1;
			slotpers[i] = 1;
			return i;
		}
	}
	return -1;
}

void freeUpPersonCache() {
	for (int i = 0; i < CACHEPERS; i++) 
		if (inusepers[i] == 0) 
			slotpers[i] = 0;
}

void decizieAddInCache(memFisierPersonal* pers, int decizie, char* data) {
	int len;

	switch (decizie)
	{
	case 1:
		if (pers->ptrn->nume != nullptr && pers->ptrn->next == nullptr)
			pers->nextPrieten();
		else if (pers->ptrn->nume != nullptr)
			pers->ptrn = pers->ptrn->next;
		len = strlen(data);
		pers->ptrn->nume = new char[len+1];
		strncpy(pers->ptrn->nume, data , len);
		pers->ptrn->nume[len] = 0;
		break;
	case 2:
		if (pers->ptrn->opinie != -1 && pers->ptrn->next==nullptr)
			pers->nextPrieten();
		else if (pers->ptrn->opinie != -1)
			pers->ptrn = pers->ptrn->next;
   		len = atoi(data);
		pers->ptrn->opinie = len;
	}
	
}

void formatareDateFisierToRam(string path, memFisierPersonal* percache) {
	ifstream in(path, ios::binary);
	string str;
	int i=0,len=0;
	if (!in)
		return;

	// get length of file:
	in.seekg(0, in.end);
	len = in.tellg();
	in.seekg(0, in.beg);

	percache->memblock = new char[len];
	in.read(percache->memblock, len);
	percache->memblock[len] = '\0';
	in.close();
	cin.get();
	 
	/*{//ii debugging aicia 
		system("cls");
		cout << percache->memblock << endl << "/////" << endl << len << endl << "/////" << endl;
		for (int i = 0; i <= len; i++)
			cout << (char)percache->memblock[i] << ":" << (int)percache->memblock[i] << endl;
	}*/

	///
	///se cauta unde sunt numerele pentru serie 
	///


	struct list{
		int i;
		list* next=nullptr;
	};
	list* start=nullptr , *ptr;

	if (percache->memblock[0] != 13) {
		list* aux;
		aux = start;
		start = new list;
		start->i = 0;
		start->next = aux;
	}
	for(int i=0;i<len;i++)
		if (percache->memblock[i] == 10) {
			list* aux;
			aux = start;
			start = new list;
			start->i = i+1;
			start->next = aux;
		}

	//poate mai citeste de la final
	if (percache->memblock[start->i] == 0) {
		list* aux;
		aux = start;
		start = start->next;
		delete aux;
	}
	
	///
	///aici se termina 
	///
	
	
	ptr = start;
	while (start != nullptr) {
		cout << start->i << " pozita , caracter : " << percache->memblock[start->i] << endl;
		start = start->next;
	}
	start = ptr;

	while (start != nullptr) {
 		int decizie = 0, i ,j;
		//cauta unde incepe acolada
		for (i = start->i + 1; percache->memblock[i] != '{' && i < len; i++);

		//preia numarul
		char *nrdec;
		i = i - start->i;
		nrdec = new char[i + 1];
		strncpy(nrdec,percache->memblock+start->i,i);
		nrdec[i] = 0;
		decizie = atoi(nrdec);

		i = i + start->i;
		int nrvirg = 0;
		//cauta unde se inchide acolada
		for (j = i + 1; percache->memblock[j] != '}' && j < len; j++)
			if (percache->memblock[j] == ',')
				nrvirg++;

		//citeste dintre acolade
		bool inghilimele = 0;
		int x, y;
		for (i = i + 1; i < j; i++) {
			if (percache->memblock[i] == '"' && inghilimele == 1) {
				inghilimele ^= 1;
				y = i;
				char* info = new char[y - x + 1]{ 0 };
				strncpy(info, percache->memblock+x+1,y-1-x);
				decizieAddInCache(percache,decizie,info);
			}
			else if (percache->memblock[i] == '"') {
				inghilimele ^= 1;
				x = i;
			}

		}
		percache->ptrn = percache->startPtrn;
		//cout << nrdec << endl;
		list* aux;
		aux = start;
		start = start->next;
		delete aux;
	}

}

bool existentInCache(UserInfo per) {
	for (int i = 0; i < CACHEPERS; i++)
		if (slotpers[i] == 1 && mempers[i]->persoana.UserId == per.UserId && mempers[i]->persoana.UserId == per.UserId)
			return 1;

	return 0;
}

void addPersonCache(UserInfo per) {
	if (existentInCache(per))
		return;
	string perFile = DB.Users + to_string(per.UserId) + '_' + per.UserName + ".txt";
	int slotliber = slotFreePersonCache();
	if (slotliber == -1){ 
		freeUpPersonCache();
		slotliber = slotFreePersonCache();
	}
	mempers[slotliber] = new memFisierPersonal;
	mempers[slotliber]->persoana=per;
	slotpers[slotliber] = 1;
	formatareDateFisierToRam(perFile,mempers[slotliber]);
}

void AddUser(const string nume) {
	ifstream in(DB.nrdeutilizatori);
	in >> DB.nrUtilizatori >> DB.lastid;
	in.close();
	DB.nrUtilizatori++;
	DB.lastid++;

	ofstream out(DB.nrdeutilizatori);
	out << DB.nrUtilizatori << ' ' << DB.lastid;
	out.close();
	
	string str = DB.Users + to_string(DB.lastid) + '_' + nume + ".txt";
	persoana.UserId = DB.lastid;
	persoana.UserName = nume;
	out.open(str);
	out.close();
}

void createFisier(string nmfisier) {
	ofstream x;
	x.open(nmfisier);
	x.close();
}

void deleteFile(const char* nmfisier) {
	if (remove(nmfisier) != 0)
		printf("eroare in stergerea fisierului");
}

void adaugaPersoane() {
	system("cls");
	cout << "1)" << endl;
	string numePersoana;
	cout << endl << "   Introduceti numele persoanei " << endl << endl << " :  ";
	getline(cin, numePersoana);
	UserInfo persoanaloc;
	returneazaPersoanaDateDB(numePersoana, persoanaloc);
	if (persoanaloc.UserId == -1)
		AddUser(numePersoana);
	else {
		cout << "persoana deja exista in database" << endl;
		cin.get();
	}
}

void returneazaPersoanaDateDB(string numePersoana, UserInfo &Persoana) {
	bool ver = 1; string pFilename; int i;
	for (auto& p : directory_iterator(DB.Users)) {
		pFilename = p.path().filename().u8string();
		i = pFilename.find('_');
		if (pFilename.substr(i + 1) == numePersoana + ".txt") {
			ver = 0;
			break;
		}
	}

	if (ver)
		return;

	Persoana.UserName = numePersoana;
	Persoana.UserId = stoi(pFilename.substr(0, i));
}

void returneazaPersoanaPath(string numePersoana, string &path) {
	/*bool ver = 1; string pFilename; int i;
	for (auto& p : directory_iterator(DB.Users)) {
		pFilename = p.path().filename().u8string();
		i = pFilename.find('_');
		if (pFilename.substr(i + 1) == numePersoana + ".txt") {
			ver = 0;
			pFilename = p.path().u8string();
			break;
		}
	}

	if (ver)
		return;
	
	path = pFilename;*/
}

int locPersInCache(string numePersoana){
	for (int i = 0; i < CACHEPERS; i++) {
		if (slotpers[i])
			if (mempers[i]->persoana.UserId != -1 && mempers[i]->persoana.UserName == numePersoana)
				return i;
	}

	return -1;
}

void cautaPersoana(){
	system("cls");
	string numePersoana;
	cout << "2)" << endl;
	cout << endl << "   Introduceti numele persoanei " << endl << endl << " :  ";
	getline(cin, numePersoana);
	int pozCachePers = locPersInCache(numePersoana);
	UserInfo persoanaLoc;
	if(pozCachePers!=-1){
		persoanaLoc = mempers[pozCachePers]->persoana;
	}
	else {
		returneazaPersoanaDateDB(numePersoana, persoanaLoc);
	}

	if (persoanaLoc.UserId == -1) {
		cout << "nu exista aceasta persoana " << endl;
		cin.get();
		return;
	}
	persoana = persoanaLoc;
	cout << "numele persoanei : " << persoana.UserName << endl;
	cout << "Id persoanei     : " << persoana.UserId << endl;
	
	// mai bine spus , daca vrei sa il bagi in cache
	cout << endl << " doriti cumva sa mai faceti operatii asupra acestei persoane ?" << endl<<"y/n :";
	char x; cin >> x;
	if(x=='y'){
		addPersonCache(persoana);
	}
	
	cin.get();
	cin.get();
}

void afisareDecizii() {
	cout << endl;
	int i = 1;
	while (opPosP->next != nullptr) {
		cout << i << ") " << opPosP->posibilitate << endl;
		opPosP = opPosP->next;
		i++;
	}
	opPosP = opPos;
}

//bool decizieAfisare(int n,string str){
//	switch (n) {
//		case 1:
//			cout << str << " ,";
//			break;
//		case 2:	
//			cout << str << " ,";
//			break;
//		default:
//			return 0;
//	}
//	return 1;
//}
//
//void tipDeDecizie(int n) {
//	switch (n) {
//	case 1:
//		cout << " prietenii acestuia sunt :";
//		break;
//	case 2:
//		cout << " cat de bine se intelege cu fiecare :";
//		break;
//	default:
//		cout << " eroare " << endl;
//	}
//}

void executareDecizie(short n, string nume) {
	/*fstream fs;
	string str;
	returneazaPersoanaPath(nume, str);
	fs.open(str, fstream::in | fstream::out);*/
	switch (n) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
}

//int posibilExecutareDecizie(string fisier) {
//	ifstream in(fisier);
//	bool work = 1;
//	int decizie = 0;
//	string data;
//	while (getline(in, data) && work) {
//		if (data[0] != ' ') {
//			decizie = stoi(data);
//			cout << endl;
//			tipDeDecizie(decizie);
//		}
//		else {
//			work = decizieAfisare(decizie, data);
//		}
//	}
//}

void citireFisierPersonal(string fisier){
	/*ifstream in(fisier);
	bool work = 1;
	int decizie = 0;
	string data;
	while ( getline(in,data) && work) {
		if (data[0] != ' ') {
			decizie = stoi(data);
			cout << endl;
			tipDeDecizie(decizie);
		}
		else {
			work=decizieAfisare(decizie,data);
		}
	}*/
}

void afisareDatePersoana() {
	system("cls");
	string numePersoana,numefisier;
	cout << "3)" << endl;
	cout << endl << "   Introduceti numele persoanei " << endl << endl << " :  ";
	getline(cin, numePersoana);
	
	cin.get();
}

//void modificareaDatelorPersoanei() {
//	system("cls");
//	cout << "4)" << endl;
//	string numePersoana, numefisier;
//
//	cout << endl << "   Introduceti numele persoanei " << endl << endl << " :  ";
//	getline(cin, numePersoana);
//	//cout << endl << "ce doriti sa schimbati ?";
//
//	dateModificariPosibilitati();
//	bitset<1> posibilitati;
//	afisareDecizii();
//	
//	short raspuns;
//	cout << " raspuns : "; cin >> raspuns;
//	executareDecizie(raspuns,numePersoana);
//
//	cin.get();
//	cin.get();
//}