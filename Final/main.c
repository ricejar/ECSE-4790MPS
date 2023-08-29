
#include "main.h"
#include "aes.h"
#include "gcm.h"
#include "md.h"
#include "pkcs5.h"
#include "ecdh.h"

RNG_HandleTypeDef hrng;
char input1;
char inputWords[70];
uint32_t rngNumber;
uint32_t tempNumber;
unsigned char secret_key[256] = {0};
unsigned char hash_key[128] = {0};
mbedtls_aes_context ctx;
mbedtls_gcm_context ctx_gcm;
mbedtls_md_context_t ctx1;

static const uint8_t addition[4] = {0xfe, 0xa5,0x5a,0xef};


// Private function prototypes
static void MX_RNG_Init(void);
void getRandomKey(int length);
void getRandomKey128(int length);
void AESCTR(void);
void AESGCM(void);


int main(void)
{
	Sys_Init();

	MX_RNG_Init();
	MX_MBEDTLS_Init();
	printf("\033[2J\033[;H");
	//printf("start\r\n");
	fflush(stdout);
	HAL_Delay(1000);

	while (1)
	{
		printf("press 1 for AES-CTR, press 2 for AES-CTR\r\n");
		input1 = getchar();
		putchar(input1);
		printf("\r\n");
//		rngNumber = HAL_RNG_GetRandomNumber(&hrng);
//		printf("RNG: hex = %x, dec = %d \r\n",rngNumber,rngNumber);
		//getRandomKey(256);
		if (input1 == 49){
			AESCTR();
		}else if (input1 == 50){
			AESGCM();
		}
	}
}


void AESCTR(){

	mbedtls_aes_init(&ctx);
	char input[70] = {0};
	char output[70] = {0};
	char backoutput[70] = {0};
	unsigned char nounce_counter[16] = {0};
	unsigned char stream_block[16]   = {0};
	int lengths = 0;
	size_t nc_off = 0;

	while(1){
        // Generate random key
		getRandomKey(256);
		printf("Input a message: \r\n");
		lengths = uart_getline(&USB_UART,input,70);
		printf("Input message: %s\r\n",input);

		// Apply the key for encrypt
		if (mbedtls_aes_setkey_enc(&ctx, secret_key, 256) != 0){
			printf("Generate Key Failure!\r\n");
			return;
		}
        // Encrypt the plaintext
		if (mbedtls_aes_crypt_ctr(&ctx, lengths, &nc_off, nounce_counter, stream_block, &input, &output) != 0){
			printf("Encrypt the Plaintext Failure\r\n");
			return;
		}

		printf("The encrypted message is: %s\r\n", output);
        // clean the set
		memset(nounce_counter, 0, sizeof(nounce_counter));
		memset(stream_block,   0, sizeof(stream_block));

		nc_off = 0;
        // Decrypt the message
		if (mbedtls_aes_crypt_ctr(&ctx, lengths, &nc_off, nounce_counter, stream_block, &output, &backoutput) != 0){
			printf("Decrypt the Plaintext Failure\r\n");
			return;
		}
		printf("The decrypted message is: %s\r\n", backoutput);
        // clear all the arrays
		memset(input, 		   0, sizeof(input));
		memset(output, 		   0, sizeof(output));
		memset(backoutput,     0, sizeof(backoutput));
		memset(nounce_counter, 0, sizeof(nounce_counter));
		memset(stream_block,   0, sizeof(stream_block));
		nc_off = 0;
		printf("\r\n");
	}
}

void AESGCM(){
    // Initialization
	mbedtls_gcm_init(&ctx_gcm);
	mbedtls_md_init(&ctx1);
	char input[20] = {0};   // input password
	char backoutput[70] = {0}; // Input message
	int lengths = 0;
    int messageLen = 0;
	char inputMessage[50];
	char messageEc[50];
	int correct = 0;
    // result of hashing
    unsigned char midkey[256] = {0};
    uint8_t tagBuf[16];
    uint8_t iv[12] = {1,2,3,4,5,6,7,8,9,0,1,2};

    // Generate random salt
	getRandomKey128(128);
    // get password
	printf("Input a password: \r\n");
	lengths = uart_getline(&USB_UART,input,20);
	printf("Password: %s\r\n",input);

    // Initialization of the hashing funciton
	extern const mbedtls_md_info_t mbedtls_sha256_info;
	int resultCode = mbedtls_md_setup(&ctx1, &mbedtls_sha256_info, 1);
    // get the key for encrypt
	correct = mbedtls_pkcs5_pbkdf2_hmac(&ctx1,input,lengths,hash_key,128,5,256,midkey);
	if (correct != 0){
	    	printf("PBKDF2 derive failure\r\n");
	    	return;
	    }
    // set the key for encrypt
    if(mbedtls_gcm_setkey(&ctx_gcm, MBEDTLS_CIPHER_ID_AES, midkey, 256) != 0){
    	printf("Generate Key Failure!\r\n");
    	return;
    }
    // clean the message buffer
    memset(inputMessage, 		   0, sizeof(inputMessage));
    // get the message
    printf("Input a message: \r\n");
    lengths = uart_getline(&USB_UART,inputMessage,50);
    messageLen = lengths;
    printf("Input message: %s\r\n",inputMessage);

    // Encrpyt the message
    if(mbedtls_gcm_crypt_and_tag(&ctx_gcm, MBEDTLS_GCM_ENCRYPT, lengths, iv, 12, addition, 4,inputMessage, messageEc, 16, tagBuf) != 0){
    	printf("Encrypt the Plaintext Failure!\r\n");
    	return;
    }
    mbedtls_gcm_free(&ctx_gcm);

    // clear the data buffer of key and input password
    memset(input, 		   0, sizeof(input));
    memset(midkey, 		   0, sizeof(midkey));

    // Get the password again
    printf("Input the password: \r\n");
    lengths = uart_getline(&USB_UART,input,50);
    printf("Password: %s\r\n",input);
    
    // Generate the key with same random salt
    correct = mbedtls_pkcs5_pbkdf2_hmac(&ctx1,input,lengths,hash_key,128,5,256,midkey);
    if (correct != 0){
    	printf("PBKDF2 derive failure\r\n");
    	return;
    }

    // Generate the key
    if(mbedtls_gcm_setkey(&ctx_gcm, MBEDTLS_CIPHER_ID_AES, midkey, 256) != 0){
        printf("Generate Key Failure!\r\n");
        return;
    }

    // Decrypt the message
    correct = mbedtls_gcm_auth_decrypt(&ctx_gcm,messageLen,iv,12,addition,4,tagBuf, 16,messageEc,backoutput);
    printf("Decrypt message: %s\r\n", backoutput);

    mbedtls_gcm_init(&ctx_gcm);

    // clear data bufer
    memset(input, 		   0, sizeof(input));
    memset(midkey, 		   0, sizeof(midkey));
    memset(hash_key,       0, sizeof(hash_key));

    printf("\r\n");
}

// Generate random key
void getRandomKey(int length){
	for (int i =0; i<length; i++){
		if ((i+1)%32 == 1){
			rngNumber = HAL_RNG_GetRandomNumber(&hrng);
		}
		secret_key[i] = rngNumber % 2;
		rngNumber = rngNumber / 2;
	}
}

// Generate random key
void getRandomKey128(int length){
	for (int i =0; i<length; i++){
		if ((i+1)%32 == 1){
			rngNumber = HAL_RNG_GetRandomNumber(&hrng);
		}
		hash_key[i] = rngNumber % 2;
		rngNumber = rngNumber / 2;
	}
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{
	hrng.Instance = RNG;
	if (HAL_RNG_Init(&hrng) != HAL_OK)
	{
		printf("Failed to initialize RNG\r\n");
		while (1);
	}
}
