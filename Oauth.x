/*
Interfata pentru Oauth.
Procedurile sunt declarate in ordinea in care sunt utilizate.
Pentru o usoara intelegere, am declarat structurile de date in ordinea folosirii.
*/

// pasez id ul utilizatorului si un int care simbolizeaza daca vrea sa reinnoiasca automat
struct request_authorization_args{
    string user_id<>;
    int auto_renew;
};
// raspunsul este tokenul de autentificare sau o eroare
struct request_auth_res{
    string auth_token<>;
    string error<>;
};
// clientul trimite doar tokenul de autorizare
struct approve_request_token_args {
    string auth_token<>;
};
// datele sunt reprezentate de tokenul de autorizare plus fisierele si permisiunile
struct SignedData{
    string auth_token<>;
    string files_and_perms<>;
};
// raspunsul consta in datele semnate, confirmarea daca sunt semnate, si in caz de 
// neautorizare de la user, o eroare
struct approve_request_token_res{
    SignedData signed_data;
    bool is_signed;
    string error<>;
};
// clientul trimite signed_data, un bool care precizeaza daca sunt semnate si id-ul userului
struct request_access_token_args{
    string user_id<>;
    SignedData signed_data;
    bool is_signed;
};
// raspunsul consta in tokenul de acces, refresh, o valabilitate si eroare
struct request_access_token_res{
    string access_token<>;
    string refresh_token<>;
    int valability;
    string error<>;
};
// pentru a permite o operatie, clientul trimite tipul operatiei, numele fisierului si tokenul de acces
struct validate_delegated_action_args{
    string op_type<>;
    string filename<>;
    string access_token<>;
};
// pentru refresh, raspunsul consta in exact acelasi tip de structura ca la request_access_token
// am vrut sa fie denumite diferit, pentru ca sa nu le incurc
struct refresh_access_token_res{
    string error<>;
    string refresh_token<>;
    string access_token<>;
    int availability;
};
// clientul trimite doar tokenul de refresh
struct refresh_access_token_args{
    string refresh_token<>;
};

program AUTH_PROG {
    version OAUTH {
        request_auth_res request_authorization(request_authorization_args) = 1;
        approve_request_token_res approve_request_token(approve_request_token_args) = 2;
        request_access_token_res request_access_token(request_access_token_args) = 3;
        string validate_delegated_action(validate_delegated_action_args) = 4;
        refresh_access_token_res refresh_access_token(refresh_access_token_args) = 5;
    } = 1;
} = 0x12321215;