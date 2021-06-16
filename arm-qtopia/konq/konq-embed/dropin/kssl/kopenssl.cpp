/* This file is part of the KDE libraries
   Copyright (C) 2001 George Staikos <staikos@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <config.h>

#if defined(KONQ_DYNAMIC_SSL)
#include "../../kdesrc/kssl/kopenssl.cc"
#else

#include <kdebug.h>
#include <kconfig.h>

#include <stdio.h>
#include "kopenssl.h"

#ifdef HAVE_SSL
#define crypt _openssl_crypt
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/asn1.h>
#include <openssl/pkcs7.h>
#include <openssl/pkcs12.h>
#undef crypt
#endif    


bool KOpenSSLProxy::hasLibSSL() const {
#ifdef HAVE_SSL
    return true;
#else
    return false;
#endif    
}


bool KOpenSSLProxy::hasLibCrypto() const {
#ifdef HAVE_SSL
    return true;
#else
    return false;
#endif    
}


void KOpenSSLProxy::destroy() {
  delete this;
  _me = NULL;
}


KOpenSSLProxy::KOpenSSLProxy() {
#ifdef HAVE_SSL
    // Initialize the library (once only!)
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
#endif
}


KOpenSSLProxy::~KOpenSSLProxy() {
}


KOpenSSLProxy* KOpenSSLProxy::_me = NULL;


// FIXME: we should check "ok" and allow this to init the lib if !ok.

KOpenSSLProxy *KOpenSSLProxy::self() {
#ifdef HAVE_SSL
   if (!_me) {
      _me = new KOpenSSLProxy;
   }
#endif
   return _me;
}


#ifdef HAVE_SSL

int KOpenSSLProxy::SSL_connect(SSL *ssl) {
   return ::SSL_connect(ssl);
}


int KOpenSSLProxy::SSL_read(SSL *ssl, void *buf, int num) {
   return ::SSL_read(ssl, (char *)buf, num);
}


int KOpenSSLProxy::SSL_write(SSL *ssl, const void *buf, int num) {
   return ::SSL_write(ssl, (char *)buf, num);
}


SSL *KOpenSSLProxy::SSL_new(SSL_CTX *ctx) {
   return ::SSL_new(ctx);
}


void KOpenSSLProxy::SSL_free(SSL *ssl) {
   ::SSL_free(ssl);
}


int KOpenSSLProxy::SSL_shutdown(SSL *ssl) {
   return ::SSL_shutdown(ssl);
}


SSL_CTX *KOpenSSLProxy::SSL_CTX_new(SSL_METHOD *method) {
   return ::SSL_CTX_new(method);
}


void KOpenSSLProxy::SSL_CTX_free(SSL_CTX *ctx) {
   ::SSL_CTX_free(ctx);
}


int KOpenSSLProxy::SSL_set_fd(SSL *ssl, int fd) {
   return ::SSL_set_fd(ssl, fd);
}


int KOpenSSLProxy::SSL_pending(SSL *ssl) {
   return ::SSL_pending(ssl);
}


int KOpenSSLProxy::SSL_CTX_set_cipher_list(SSL_CTX *ctx, const char *str) {
   return ::SSL_CTX_set_cipher_list(ctx, str);
}


void KOpenSSLProxy::SSL_CTX_set_verify(SSL_CTX *ctx, int mode,
                              int (*verify_callback)(int, X509_STORE_CTX *)) {
   ::SSL_CTX_set_verify(ctx, mode, verify_callback);
}


int KOpenSSLProxy::SSL_use_certificate(SSL *ssl, X509 *x) {
   return ::SSL_use_certificate(ssl, x);
}


SSL_CIPHER *KOpenSSLProxy::SSL_get_current_cipher(SSL *ssl) {
   return ::SSL_get_current_cipher(ssl);
}


long KOpenSSLProxy::SSL_ctrl(SSL *ssl,int cmd, long larg, char *parg) {
   return ::SSL_ctrl(ssl, cmd, larg, parg);
}


int KOpenSSLProxy::RAND_egd(const char *path) {
   return ::RAND_egd(path);
}


SSL_METHOD *KOpenSSLProxy::TLSv1_client_method() {
   return ::TLSv1_client_method();
}


SSL_METHOD *KOpenSSLProxy::SSLv2_client_method() {
   return ::SSLv2_client_method();
}


SSL_METHOD *KOpenSSLProxy::SSLv3_client_method() {
   return ::SSLv3_client_method();
}


SSL_METHOD *KOpenSSLProxy::SSLv23_client_method() {
   return ::SSLv23_client_method();
}


X509 *KOpenSSLProxy::SSL_get_peer_certificate(SSL *s) {
   return ::SSL_get_peer_certificate(s);
}


int KOpenSSLProxy::SSL_CIPHER_get_bits(SSL_CIPHER *c,int *alg_bits) {
   return ::SSL_CIPHER_get_bits(c, alg_bits);
}


char * KOpenSSLProxy::SSL_CIPHER_get_version(SSL_CIPHER *c) {
   return ::SSL_CIPHER_get_version(c);
}


const char * KOpenSSLProxy::SSL_CIPHER_get_name(SSL_CIPHER *c) {
   return ::SSL_CIPHER_get_name(c);
}


char * KOpenSSLProxy::SSL_CIPHER_description(SSL_CIPHER *c,char *buf,int size) {
   return ::SSL_CIPHER_description(c,buf,size);
}


X509 * KOpenSSLProxy::d2i_X509(X509 **a,unsigned char **pp,long length) {
   return ::d2i_X509(a,pp,length);
}


int KOpenSSLProxy::i2d_X509(X509 *a,unsigned char **pp) {
   return ::i2d_X509(a,pp);
}


int KOpenSSLProxy::X509_cmp(X509 *a, X509 *b) {
   return ::X509_cmp(a,b);
}


X509_STORE *KOpenSSLProxy::X509_STORE_new(void) {
   return ::X509_STORE_new();
}


void KOpenSSLProxy::X509_STORE_free(X509_STORE *v) {
   ::X509_STORE_free(v);
}


X509_STORE_CTX *KOpenSSLProxy::X509_STORE_CTX_new(void) {
   return ::X509_STORE_CTX_new();
}


void KOpenSSLProxy::X509_STORE_CTX_free(X509_STORE_CTX *ctx) {
   ::X509_STORE_CTX_free(ctx);
}


int KOpenSSLProxy::X509_verify_cert(X509_STORE_CTX *ctx) {
   return ::X509_verify_cert(ctx);
}


void KOpenSSLProxy::X509_free(X509 *a) {
   ::X509_free(a);
}


char *KOpenSSLProxy::X509_NAME_oneline(X509_NAME *a,char *buf,int size) {
   return ::X509_NAME_oneline(a,buf,size);
}


X509_NAME *KOpenSSLProxy::X509_get_subject_name(X509 *a) {
   return ::X509_get_subject_name(a);
}


X509_NAME *KOpenSSLProxy::X509_get_issuer_name(X509 *a) {
   return ::X509_get_issuer_name(a);
}


X509_LOOKUP *KOpenSSLProxy::X509_STORE_add_lookup(X509_STORE *v, X509_LOOKUP_METHOD *m) {
   return ::X509_STORE_add_lookup(v,m);
}


X509_LOOKUP_METHOD *KOpenSSLProxy::X509_LOOKUP_file(void) {
   return ::X509_LOOKUP_file();
}


int KOpenSSLProxy::X509_LOOKUP_ctrl(X509_LOOKUP *ctx, int cmd, const char *argc, long argl, char **ret) {
   return ::X509_LOOKUP_ctrl(ctx,cmd,argc,argl,ret);
}


void KOpenSSLProxy::X509_STORE_CTX_init(X509_STORE_CTX *ctx, X509_STORE *store, X509 *x509, STACK_OF(X509) *chain) {
   ::X509_STORE_CTX_init(ctx,store,x509,chain);
}


void KOpenSSLProxy::CRYPTO_free(void *x) {
   ::CRYPTO_free(x);
}


X509 *KOpenSSLProxy::X509_dup(X509 *x509) {
   return ::X509_dup(x509);
}


BIO *KOpenSSLProxy::BIO_new_fp(FILE *stream, int close_flag) {
   return ::BIO_new_fp(stream, close_flag);
}


int KOpenSSLProxy::BIO_free(BIO *a) {
   return ::BIO_free(a);
}


int KOpenSSLProxy::PEM_write_bio_X509(BIO *bp, X509 *x) {
    return ::PEM_ASN1_write_bio ((int (*)())::i2d_X509, PEM_STRING_X509, bp, (char *)x, NULL, NULL, 0, NULL, NULL);
}


ASN1_METHOD *KOpenSSLProxy::X509_asn1_meth(void) {
    return ::X509_asn1_meth();
}


int KOpenSSLProxy::ASN1_i2d_fp(FILE *out,unsigned char *x) {
    return ::ASN1_i2d_fp((int (*)())::i2d_ASN1_HEADER, out, x);
}


int KOpenSSLProxy::X509_print(FILE *fp, X509 *x) {
   return ::X509_print_fp(fp, x);
}


PKCS12 *KOpenSSLProxy::d2i_PKCS12_fp(FILE *fp, PKCS12 **p12) {
   return ::d2i_PKCS12_fp(fp, p12);
}

 
int KOpenSSLProxy::PKCS12_newpass(PKCS12 *p12, char *oldpass, char *newpass) {
    return ::PKCS12_newpass(p12, oldpass, newpass);
}

 
int KOpenSSLProxy::i2d_PKCS12_fp(FILE *fp, PKCS12 *p12) {
    return ::i2d_PKCS12_fp(fp, p12);
}


PKCS12 *KOpenSSLProxy::PKCS12_new(void) {
   return ::PKCS12_new();
}


void KOpenSSLProxy::PKCS12_free(PKCS12 *a) {
   ::PKCS12_free(a);
}


int KOpenSSLProxy::PKCS12_parse(PKCS12 *p12, const char *pass, EVP_PKEY **pkey,
                    X509 **cert, STACK_OF(X509) **ca) {
   return ::PKCS12_parse (p12, pass, pkey, cert, ca);
}


void KOpenSSLProxy::EVP_PKEY_free(EVP_PKEY *x) {
   ::EVP_PKEY_free(x);
}


int KOpenSSLProxy::SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey) {
    return ::SSL_CTX_use_PrivateKey(ctx,pkey);
}


int KOpenSSLProxy::SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x) {
   return ::SSL_CTX_use_certificate(ctx,x);
}


int KOpenSSLProxy::SSL_get_error(SSL *ssl, int rc) {
   return ::SSL_get_error(ssl,rc);
}

STACK_OF(X509) *KOpenSSLProxy::SSL_get_peer_cert_chain(SSL *s) {
   return SSL_get_peer_cert_chain(s);
}


void KOpenSSLProxy::sk_free(STACK *s) {
   sk_free(s);
}


int KOpenSSLProxy::sk_num(STACK *s) {
   return sk_num(s);
}

 
char *KOpenSSLProxy::sk_value(STACK *s, int n) {
   return sk_value(s, n);
}


void KOpenSSLProxy::X509_STORE_CTX_set_chain(X509_STORE_CTX *v, STACK_OF(X509)* x) {
   X509_STORE_CTX_set_chain(v,x);
}


STACK* KOpenSSLProxy::sk_dup(STACK *s) {
   return sk_dup(s);
}


STACK* KOpenSSLProxy::sk_new(int (*cmp)()) {
   return sk_new(cmp);
}


int KOpenSSLProxy::sk_push(STACK* s, char* d) {
   return sk_push(s,d);
}
#endif

#endif

