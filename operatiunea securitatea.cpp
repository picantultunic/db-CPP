#include <iostream>
#include "libsec.h"

int meniu() {
	system("cls");
	cout << endl << "<<baza de date cu oameni>>" << endl << endl;

	if (persoana.UserId != -1)
		cout << "userul in memorie :" << persoana.UserName << endl << endl;
	else
		cout << endl << endl;

	cout << "   aveti la alegere mai multe obtiuni :" << endl;
	cout << "0) Exit" << endl;
	cout << "1) Adaugati persoane in baza de date" << endl;
	cout << "2) Verificati date despre acea persoana" << endl;
	cout << "3) Accesarea datelor despre persoanei" << endl;
	cout << "4) Modificarea datelor persoanei" << endl;
	cout << "5) Adaugarea unor date noi" << endl;
	cout << "6) Afisare Cache" << endl;
	cout << "   Raspunsul dumneavoastra :";

	int raspuns;
	cin >> raspuns;
	cin.get();

	switch (raspuns)
	{
	case 1:
		adaugaPersoane();
		break;
	case 2:
		cautaPersoana();
		break;
	case 3:
		//afisareDatePersoana();
		break;
	case 4:
		//modificareaDatelorPersoanei();
		break;
	case 5:
		break;
	case 6:
		afisarecache();
		break;
	default:
		system("cls");
		cout << "o zi buna" << endl;
		return 0;
	}
	return 1;
}

int main()
{
	string fsetupname = "setup.txt";
	DBinitializare(fsetupname);
	if (!DB.activ) {
		cout << "se pare ca nu o reusit sa mearca databaseul , ne pare rau " << endl;
		return 0;
	}

	while (meniu()) {}
	return 0;
}
