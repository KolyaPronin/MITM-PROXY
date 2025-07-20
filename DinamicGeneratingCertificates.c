#include "headerFile.h"

fake_cert generate_fake_cert(char *host) {
    fake_cert args_to_return;

    // Генерация ключа
    EVP_PKEY *evp_pkey = EVP_PKEY_new();
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(rsa, 2048, e, NULL);
    EVP_PKEY_assign_RSA(evp_pkey, rsa);

    // Открытие CA-сертификата и ключа
    FILE *f_ca_cert = fopen("ca.crt", "r");
    if (!f_ca_cert) perror("fopen ca.crt");

    FILE *f_ca_key = fopen("ca.key", "r");
    if (!f_ca_key) perror("fopen ca.key");

    X509 *ca_cert = PEM_read_X509(f_ca_cert, NULL, NULL, NULL);
    EVP_PKEY *ca_key = PEM_read_PrivateKey(f_ca_key, NULL, NULL, NULL);

    // Создание нового X509
    X509 *x509 = X509_new();

    BIGNUM *serial = BN_new();
    BN_rand(serial, 64, 0, 0);
    ASN1_INTEGER *asn1_serial = BN_to_ASN1_INTEGER(serial, NULL);
    X509_set_serialNumber(x509, asn1_serial);

    X509_set_version(x509, 2);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 60 * 60 * 24 * 365); // 1 год

    X509_NAME *host_name = X509_NAME_new();
    X509_NAME_add_entry_by_txt(host_name, "CN", MBSTRING_ASC, (unsigned char*)host, -1, -1, 0);
    X509_set_subject_name(x509, host_name);
    X509_set_issuer_name(x509, X509_get_subject_name(ca_cert));
    X509_set_pubkey(x509, evp_pkey);

    X509V3_CTX ctx;
    X509V3_set_ctx(&ctx, ca_cert, x509, NULL, NULL, 0);
    X509V3_set_ctx_nodb(&ctx);

    char san_str[512];
    snprintf(san_str, sizeof(san_str), "DNS:%s", host);

    X509_EXTENSION *ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_alt_name, san_str);
    if (!ext) {
        fprintf(stderr, "Failed to create SAN extension\n");
    } else {
        X509_add_ext(x509, ext, -1);
        X509_EXTENSION_free(ext);
    }

    
    X509_sign(x509, ca_key, EVP_sha256());

    
    fclose(f_ca_key);
    fclose(f_ca_cert);
    BN_free(serial);
    ASN1_INTEGER_free(asn1_serial);
    X509_NAME_free(host_name);

    args_to_return.cert = x509;
    args_to_return.pkey = evp_pkey;
    return args_to_return;
}
