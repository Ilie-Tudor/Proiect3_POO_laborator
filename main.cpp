#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <cstring>
#include <fstream>
using namespace std;
/// articol index, cota, carte/revista, titluCarte/DenumireCD, nrexemplareTotale, nrexemplareImprumutate, luna(pentru revista);
class Tokenizer{
public:
    static vector<string> ParseCSVLine(string line){
        string cota,tip,titlu,nrExT,nrExI,luna;
        vector<string> v;
        int sepindex = -1 ;
        for(int i=0;i<line.size();i++){
            if(line[i]==','){
                string temp = line.substr(sepindex+1,i-sepindex-1);
                v.push_back(temp);
                sepindex = i;
            }
        }
        string temp = line.substr(sepindex+1,line.size());
        v.push_back(temp);
        return v;
    }
    static string CreateCSVLine(vector<string> v){
        string rez = v[0];
        for(int i=1;i<v.size();i++){
            rez+=","+v[i];
        }
        return rez;
    }
};
class Articol{
private:
    static int nrArticole;
protected:
    char m_tip;
    int m_cota = 0;
    int m_nrExemplare = 0;
public:
    virtual ~Articol(){}
    Articol(char tip){
        m_tip = tip;
        nrArticole++;
    }
    Articol(char tip, int cota, int nrExemplare){
        m_tip=tip;
        m_cota = cota;
        m_nrExemplare = nrExemplare;
        nrArticole++;
    }
    int GetCota()const{
        return m_cota;
    }
    int GetNrArticole()const{
        return nrArticole;
    }
    virtual tuple<char,int,string,string> GetInformatii() = 0;
    virtual void CitireInformatiiArticol(istream &in) = 0;
    friend istream& operator>>(istream &in, Articol & art);
};
int Articol::nrArticole;
istream& operator>>(istream &in, Articol & art){
    art.CitireInformatiiArticol(in);
}

class Carte:public Articol{
    string m_titlu;
public:
    Carte():Articol('c'){}
    Carte(int cota, int nrExemplare, string titlu):Articol('c',cota,nrExemplare){
        m_titlu = titlu;
    }
    void CitireInformatiiArticol(istream &in){
        cout<<"Cota articol: ";
        in>>m_cota;
        cout<<"Numarul de exemplare: ";
        in>>m_nrExemplare;
        cout<<"Titlul cartii: ";
        getline(in,m_titlu);
        getline(in,m_titlu);
        cout<<'\n';
    }
    tuple<char,int,string,string> GetInformatii(){
        return make_tuple(m_tip,m_nrExemplare,m_titlu,"undefined");
    }
};

class Revista:public Articol{
    string m_denumireCD;
    string m_lunaEmitere;
public:
    Revista():Articol('r'){};
    Revista(int cota, int nrExemplare, string denumireCD, string lunaEmitere):Articol('r',cota,nrExemplare){
        m_denumireCD = denumireCD;
        m_lunaEmitere = lunaEmitere;
    }
    virtual tuple<char,int,string,string> GetInformatii(){
        return make_tuple(m_tip,m_nrExemplare,m_denumireCD,m_lunaEmitere);
    }
    void CitireInformatiiArticol(istream &in){
        cout<<"Cota articol: ";
        in>>m_cota;
        cout<<"Numarul de exemplare: ";
        in>>m_nrExemplare;
        cout<<"Denumirea CD-ului: ";
        getline(in,m_denumireCD);
        getline(in,m_denumireCD);
        cout<<"Luna emiterii CD-ului: ";
        getline(in,m_lunaEmitere);
        cout<<'\n';
    }
};

template <class T>
class Biblioteca{
private:
    vector<T*> vectorArticole;
    vector<int> numarImprumutate;
public:
    Biblioteca(){}
    Biblioteca(const Biblioteca &b){
        vectorArticole = b.vectorArticole;
        numarImprumutate = b.numarImprumutate;
    }
    void AddArticol(T *art){
        for(unsigned int i=0;i<vectorArticole.size();i++){
            if(art == vectorArticole[i] || vectorArticole[i]->GetCota()==art->GetCota()){
                throw("Cota sau pointerul catre articol este duplicat\n");
                return;
            }
        }
        vectorArticole.push_back(art);
        numarImprumutate.push_back(0);
    }
    void PrintArticole()const{
        if(vectorArticole.size()==0){
            cout<<"Biblioteca este complet goala momentan\n";
        }
        for(unsigned int i=0;i<vectorArticole.size();i++){
            tuple <char,int,string,string> informatii = vectorArticole[i]->GetInformatii();
            if(get<0>(informatii)=='c'){
                cout<<"Articolul "<<i+1<<" este o carte\n";
                cout<<"Cota articolului: "<<vectorArticole[i]->GetCota()<<"\n";
                cout<<"Titulul cartii: "<<get<2>(informatii)<<"\n";
                cout<<"Numarul de exemplare disponibile in biblioteca: "<<get<1>(informatii)-numarImprumutate[i]<<"\n";
            }
            else if(get<0>(informatii)=='r'){
                cout<<"Articolul "<<i+1<<" este o revista cu CD\n";
                cout<<"Denumirea CD-ului: "<<get<2>(informatii)<<"\n";
                cout<<"Cota articolului: "<<vectorArticole[i]->GetCota()<<"\n";
                cout<<"Numarul de exemplare disponibile in biblioteca: "<<get<1>(informatii)-numarImprumutate[i]<<"\n";
            }
        }
        cout<<"\n\n\n";
    }
    void ImprumutaArticol(int cota){
        bool inBiblioteca = false;
        for(unsigned int i=0;i<vectorArticole.size();i++){
            if(vectorArticole[i]->GetCota()==cota){
                inBiblioteca = true;
                if(numarImprumutate[i] < get<1>(vectorArticole[i]->GetInformatii())){
                    numarImprumutate[i]+=1;
                    return;
                }
                else{
                    throw("Nu sunt exemplare disponibile momentan\n");
                }
            }
        }
        if(!inBiblioteca){
            throw("Articolul cu cota ceruta nu face parte din oferta bibliotecii\n");
        }
    }
    void ReturneazaArticol(int cota){
        bool inBiblioteca = false;
        for(unsigned int i=0;i<vectorArticole.size();i++){
            if(vectorArticole[i]->GetCota()==cota){
                inBiblioteca = true;
                if(numarImprumutate[i]>0){
                    numarImprumutate[i]--;
                    return;
                }
                else{
                    throw("Toate exemplarele articolului respectiv sunt in biblioteca. Verificati din nou cota sau biblioteca de la care ati imprumutat\n");
                }
            }
        }
        if(!inBiblioteca){
            throw("Articolul cu cota ceruta nu face parte din oferta bibliotecii\n");
        }
    }
    vector<T*> GetArticole(){
        return vectorArticole;
    }
    vector<int> GetImprumutate(){
        return numarImprumutate;
    }
};
void MeniuInteractiv(Biblioteca<Articol> &B, bool &callback){
    cout<<"Alegeti una din optiunile:\n";
    cout<<"1 Afisare articole disponibile: introduceti tasta 1\n";
    cout<<"2 Imprumutati un articol: introduceti tasta 2\n";
    cout<<"3 Returnati un articol: introduceti tasta 3\n";
    cout<<"4 Adaugati un articol in biblioteca: introduceti tasta 4\n";
    cout<<"5 Exit: introduceti tasta 5\n";


    int comanda;
    cin>>comanda;
    cout<<'\n';
    switch(comanda){
    case 1:{
        B.PrintArticole();
        break;
    }
    case 2:{
        cout<<"Introduceti cota articolului pe care doriti sa il imprumutati:\n";
        int cota;
        cin>>cota;
        B.ImprumutaArticol(cota);
        break;
    }
    case 3:{
        cout<<"Introduceti cota articolului pe care doriti sa il returnati:\n";
        int cota;
        cin>>cota;
        B.ReturneazaArticol(cota);
        break;
    }
    case 4:{
        string tip = "nimic introdus inca";
        do{
            cout<<"Introduceti tipul articolului pe care doriti sa il adaugati: (carte sau revista): \n";
            cin>>tip;
        }
        while(tip!="carte" && tip!="revista");
        Articol *p;
        if(tip=="carte"){
            p = new Carte;
        }
        else if(tip=="revista"){
            p = new Revista;
        }
        cin>>(*p);
        B.AddArticol(p);
        break;
    }
    case 5:{
        callback = true;
        cout<<"Schimbarile la nivelul bibliotecii au fost salvate in baza de date\n";
        break;
    }
    default:{
        cout<<"Introduceti o comanda valida\n";
        break;
    }
    cout<<'\n';
    }
}

int main()
{
    Biblioteca<Articol> B;

    ifstream ifile("database.txt");
    if(ifile.is_open()){
       string line;
       vector<string> info;
       getline(ifile,line);
       getline(ifile,line);
       getline(ifile,line);
       while(!line.empty()){
            try{
                getline(ifile,line);
                if(line.empty()) break;
                info = Tokenizer::ParseCSVLine(line);
                if(info[2]=="carte"){
                    Articol * art = new Carte(stoi(info[1]),stoi(info[4]),info[3]);
                    B.AddArticol(art);
                    for(int i = 0;i<stoi(info[5]);i++){
                        B.ImprumutaArticol(stoi(info[1]));
                    }
                }
                else if(info[2]=="revista"){
                    Articol * art = new Revista(stoi(info[1]),stoi(info[4]),info[3],info[6]);
                    B.AddArticol(art);
                    for(int i = 0;i<stoi(info[5]);i++){
                        B.ImprumutaArticol(stoi(info[1]));
                    }
                }
            }
            catch(char const * x){
                cout<<x;
            }
       }

    }


    cout<<"Bun venit in Biblioteca din Grozavesti!\n";
    bool callback = false;
    while(!callback){
        try{
            MeniuInteractiv(B,callback);
        }
        catch(char const * x){
            cout<<x;
        }
    }


    ofstream ofile ("database.txt");
    if(ofile.is_open()){
        vector<Articol*> vA=B.GetArticole();
        vector<int> vI = B.GetImprumutate();
        ofile<<"Header:\n";
        ofile<<"Structura csv: articol index, cota, carte/revista, titluCarte/DenumireCD, nrexemplareTotale, nrexemplareImprumutate, luna(pentru revista)\n";
        ofile<<"Data:\n";
        for(int i = 0;i<vA.size();i++){
            /// articol index, cota, carte/revista, titluCarte/DenumireCD, nrexemplareTotale, nrexemplareImprumutate, luna(pentru revista);
            vector<string> toWrite;
            tuple <char,int,string,string> informatii= vA[i]->GetInformatii();
            toWrite.push_back(to_string(i+1));
            toWrite.push_back(to_string(vA[i]->GetCota()));
            if(get<0>(informatii)=='c'){
                toWrite.push_back("carte");
            }
            else{
                toWrite.push_back("revista");
            }
            toWrite.push_back(get<2>(informatii));
            toWrite.push_back(to_string(get<1>(informatii)));
            toWrite.push_back(to_string(vI[i]));
            if(get<0>(informatii)=='r'){
                toWrite.push_back(get<3>(informatii));
            }
            ofile<<Tokenizer::CreateCSVLine(toWrite)<<'\n';
        }
        ofile.close();
    }


    return 0;
}
