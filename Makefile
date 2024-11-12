build:
	# rpcgen -C -a Oauth.x
	gcc -o server Oauth_server.c Oauth_svc.c Oauth_xdr.c helper_Oauth.c -ltirpc -I/usr/include/tirpc -g -Wall
	gcc -o client Oauth_client.c Oauth_clnt.c Oauth_xdr.c helper_Oauth.c -ltirpc -I/usr/include/tirpc -g -Wall
clean:
	rm -f client server Oauth.h Oauth_svc.c Oauth_clnt.c Oauth_xdr.c