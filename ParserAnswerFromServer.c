#include "headerFile.h"

char *get_next_line(char *str){
    char *next = strstr(str, "\r\n");
    if (!next) return NULL;
    next += 2;
    return next;
}

dictionary dict = {0};

void headline_compare(char *headline, char *headline_message) {
    if (strcmp(headline, "Access-Control-Allow-Origin") == 0)
        strcpy(dict.access_control_allow_origin, headline_message);
    else if (strcmp(headline, "Date") == 0)
        strcpy(dict.data, headline_message);
    else if (strcmp(headline, "Pragma") == 0)
        strcpy(dict.pragma, headline_message);
    else if (strcmp(headline, "Expires") == 0)
        strcpy(dict.expires, headline_message);
    else if (strcmp(headline, "Cache-Control") == 0)
        strcpy(dict.cache_control, headline_message);
    else if (strcmp(headline, "Access-Control-Allow-Credentials") == 0)
        strcpy(dict.access_control_allow_credentials, headline_message);
    else if (strcmp(headline, "Content-Type") == 0)
        strcpy(dict.content_type, headline_message);
    else if (strcmp(headline, "Cross-Origin-Resource-Policy") == 0)
        strcpy(dict.cross_origin_resource_policy, headline_message);
    else if (strcmp(headline, "Content-Security-Policy-Report-Only") == 0)
        strcpy(dict.content_security_policy_report_only, headline_message);
    else if (strcmp(headline, "Cross-Origin-Opener-Policy-Report-Only") == 0)
        strcpy(dict.cross_origin_opener_policy_report_only, headline_message);
    else if (strcmp(headline, "Report-To") == 0)
        strcpy(dict.report_to, headline_message);
    else if (strcmp(headline, "Server") == 0)
        strcpy(dict.server, headline_message);
    else if (strcmp(headline, "Content-Length") == 0)
        strcpy(dict.content_lenght, headline_message);
}

void dictionary_from_parsing_https_answer(char *buffer) {
    sscanf(buffer, "%6s %4s %1023[^\r\n]", dict.protocol, dict.status_code, dict.message);

    char *headers_end = strstr(buffer, "\r\n\r\n");
    if (!headers_end) return;

    char *line = get_next_line(buffer); 

    while (line && line < headers_end) {
        char some_headline_1[64] = {0};
        char message_some_headline_1[256] = {0};

        int parsed = sscanf(line, "%63[^:]: %255[^\r\n]", some_headline_1, message_some_headline_1);
        if (parsed == 2) {
            headline_compare(some_headline_1, message_some_headline_1);
        }

        line = get_next_line(line);
    }

    FILE *out = fopen("parsed_headers.txt", "w");
    if (out) {
        fprintf(out, "protocol: %s\n", dict.protocol);
        fprintf(out, "status_code: %s\n", dict.status_code);
        fprintf(out, "message: %s\n", dict.message);
        fprintf(out, "access_control_allow_origin: %s\n", dict.access_control_allow_origin);
        fprintf(out, "data: %s\n", dict.data);
        fprintf(out, "pragma: %s\n", dict.pragma);
        fprintf(out, "expires: %s\n", dict.expires);
        fprintf(out, "cache_control: %s\n", dict.cache_control);
        fprintf(out, "access_control_allow_credentials: %s\n", dict.access_control_allow_credentials);
        fprintf(out, "content_type: %s\n", dict.content_type);
        fprintf(out, "cross_origin_resource_policy: %s\n", dict.cross_origin_resource_policy);
        fprintf(out, "content_security_policy_report_only: %s\n", dict.content_security_policy_report_only);
        fprintf(out, "cross_origin_opener_policy_report_only: %s\n", dict.cross_origin_opener_policy_report_only);
        fprintf(out, "report_to: %s\n", dict.report_to);
        fprintf(out, "server: %s\n", dict.server);
        fprintf(out, "content_lenght: %s\n", dict.content_lenght);
        fclose(out);
    } else {
        perror("fopen for parsed_headers.txt");
    }
}
