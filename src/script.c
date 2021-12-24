#include <E:/Bitcoin/bitcoinclib/includes/script.h>

void script_init(Script *self)
{
    self->size = 0;
    self->commands = calloc(self->size, 0);
}

void script_init_set(Script *self, int size, char **commands)
{
    self->size = size;
    self->commands = commands;
}

void script_init_set_p2pkh(Script *self, char *h160)
{
    self->size = 5;
    self->commands = calloc(self->size, 32);
    self->commands[0] = "118";
    self->commands[1] = "169";
    self->commands[2] = h160;
    self->commands[3] = "136";
    self->commands[4] = "172";
}
void script_serialize(char **output_buffer, Script *self)
{
    *output_buffer = calloc(100,1);

    MP_INT j;
    char *cmd;
    char *lenght_str;
    
    for (int i = 0; i < self->size; i++)
    {
        if (strlen(self->commands[i]) < 4)
        {
            mpz_init_set_str(&j, self->commands[i], 10);
            int_to_little_endian(&cmd, &j, 1);
            strcat(*output_buffer, cmd);
        }
        else 
        {
            int lenght = strlen(self->commands[i]);
            mpz_init_set_ui(&j, lenght);
            if (lenght < 75)
            {
               int_to_little_endian(&lenght_str, &j, 1); 
               strcat(*output_buffer, lenght_str);
            }
            else if (lenght < 0x100)
            {
                strcat(*output_buffer, "4c");
                int_to_little_endian(&lenght_str, &j, 1); 
                strcat(*output_buffer, lenght_str);
            }
            else if (lenght <= 520)
            {
                strcat(*output_buffer, "4d");
                int_to_little_endian(&lenght_str, &j, 2); 
                strcat(*output_buffer, lenght_str);
            }
            else 
            {
                printf("Command size too big");
                exit(-1);
            }
            strcat(*output_buffer, self->commands[i]);
        }
        
    }
    char *result_size;
    mpz_set_ui(&j, strlen(*output_buffer));
    encode_varint(&result_size, &j);
    memmove(*output_buffer+strlen(result_size), *output_buffer, strlen(*output_buffer));
    strncpy(*output_buffer, result_size, strlen(result_size));

    /* Free Memory */
    free(cmd);
    free(lenght_str);
    free(result_size);
    mpz_clear(&j);
}

void script_parse(Script *self, char **src)
{
    MP_INT i;
    decode_varint(&i, &src);
    int count = 0;
    char **cmds;
    cmds = malloc(mpz_get_ui(&i)*2 + 1);
    cmds[mpz_get_ui(&i)*2] = '\0';

    int x = 0;

    while(mpz_cmp_ui(&i, count) != 0)
    {
        char *current;
        extract(&current, &src, 1);
        count ++;
        MP_INT j;
        mpz_init_set_str(&j, current, 16);
        int k = mpz_get_ui(&j);

        if (k >= 1 && k <= 75)
        {
            char *cmd;
            extract(&cmd, &src, k);
            cmds[x] = cmd;
            count += k;   
        }
        else if(k == 76)
        {
            MP_INT data_lenght;
            char *data_lenght_str;
            extract(&data_lenght_str, &src, 1);
            little_endian_to_int(&data_lenght, data_lenght_str);
            char *cmd;
            extract(&cmd, &src, mpz_get_ui(&data_lenght));
            cmds[x] = cmd;
            count += mpz_get_ui(&data_lenght) + 1;
        }
        else if (k == 77)
        {
            MP_INT data_lenght;
            char *data_lenght_str;
            extract(&data_lenght_str, &src, 2);
            little_endian_to_int(&data_lenght, data_lenght_str);
            char *cmd;
            extract(&cmd, &src, mpz_get_ui(&data_lenght));
            cmds[x] = cmd;
            count += mpz_get_ui(&data_lenght) + 2;
        }
        else 
        {
            cmds[x] = current;
        }
        x++;
    }
    script_init_set(self, x, cmds);
}