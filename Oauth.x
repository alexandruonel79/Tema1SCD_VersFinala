struct request_authorization_args{
    string user_id<>;
    int auto_renew;
};

struct request_auth_res{
    string auth_token<>;
    string error<>;
};

struct approve_request_token_args {
    string auth_token<>;
};

struct SignedData{
    string auth_token<>;
    string files_and_perms<>;
};

struct approve_request_token_res{
    SignedData signed_data;
    bool is_signed;
    string error<>;
};

struct request_access_token_args{
    string user_id<>;
    SignedData signed_data;
    bool is_signed;
};

struct request_access_token_res{
    string access_token<>;
    string refresh_token<>;
    int valability;
    string error<>;
};

struct validate_delegated_action_args{
    string op_type<>;
    string filename<>;
    string access_token<>;
};

struct refresh_access_token_res{
    string error<>;
    string refresh_token<>;
    string access_token<>;
    int availability;
};

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