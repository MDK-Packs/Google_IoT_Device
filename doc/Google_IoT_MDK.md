Getting started with Google IoT and MDK
=======================================

This tutorial explains how to use **MDK** and the **Google_IoT_Device** pack to connect your application to the Google IoT Cloud.

Prerequisites
-------------
You require a development board with a device that is supported by [CMSIS-Drivers](http://arm-software.github.io/CMSIS_5/Driver/html/index.html). Lots of devices from STMicroelectronics and NXP do have these drivers available. Please consult the [device database](https://www.keil.com/dd2) for a list of devices and development boards.

When you have chosen your device and development board, use ![Pack Installer](./static/icons/packinst.png)[Pack Installer](https://www.keil.com/support/man/docs/uv4/uv4_ca_packinstaller.htm) to download and install the following software packs:

* **MDK-Packs::Google_IoT_Device**
* **MDK-Packs::Paho_MQTT** v1.0.0 or newer
* **MDK-Packs::IoT_Socket** v1.0.0 or newer
* **ARM::CMSIS** v5.0.1 or newer
* **ARM::mbedTLS** v1.3.1 or newer
* **Keil::MDK-Middleware** v7.4.0 or newer
* Device family pack for the selected device

**Note**:

* In order to be able to use the Google IoT Cloud Connector, you require a MDK-Professional license. &micro;Vision allows you to start a [seven day trial](http://www2.keil.com/mdk5/activation/) from within the tool.
* This tutorial assumes that you know how to work with &micro;Vision (creating projects, adding template code, configuring software components). If not, check out the [MDK Getting Started Guide](https://armkeil.blob.core.windows.net/product/gs_MDK5_4_en.pdf).
* In this tutorial, your MDK installation folder will be referenced as \<INSTALL_FOLDER>. Usually, it is C:\\Keil_v5.


In the Google Cloud IoT Console
-------------------------------

In the Google Cloud IoT console, you need to select or create a project, enable billing and the APIs, create a device registry, add a device to the registry, create key pairs and add a public key to the device. Here's how to do this:

### Select or create a project and enable billing and the APIs
Refer to [https://cloud.google.com/iot/docs/quickstart](https://cloud.google.com/iot/docs/quickstart) - section **Before you begin**

0. Sign in to your Google account.
1. In the Cloud Platform Console, go to the Manage resources page and select or create a new project.
2. Enable billing for your project.
3. Enable the Cloud IoT Core and Cloud Pub/Sub APIs.

### Create a device registry
Refer to [https://cloud.google.com/iot/docs/quickstart](https://cloud.google.com/iot/docs/quickstart) - section **Create a device registry**

1. Go to the Google Cloud IoT Core page in GCP Console. 
2. Click **Create device registry**.
3. Enter `my-registry` for the **Registry ID**.
4. Select **us-central1** for the **Cloud region**.
5. Select **MQTT** for the **Protocol**.
6. In the **Telemetry topic** dropdown list, select Create a topic.
7. In the **Create a topic** dialog, enter `my-device-events` in the **Name** field.
8. Click **Create** in the **Create a topic** dialog.
9. The **Device state topic** and **Certificate value** fields are optional, so leave them blank.
10.Click **Create** on the Cloud IoT Core page

You've just created a device registry with a Cloud Pub/Sub topic for publishing device telemetry events.

### Add a device to the registry
Refer to [https://cloud.google.com/iot/docs/quickstart](https://cloud.google.com/iot/docs/quickstart) - section **Add a device to the registry**

1. On the Registry Details page, click **Add device**.
2. Enter `my-device` for the **Device ID**.
3. Select **Allow** for **Device communication**.
4. The **Authentication** section is optional, so leave its fields blank or use the default values. The **Device metadata** field is also optional; leave it blank.
5. Click **Add**.

You've just added a device to your registry.

### Create key pairs

Google IoT Cloud supports the RSA and Elliptic Curve algorithms. Following sections explain how to generate public/private key pairs using OpenSSL command-line tools (can be executed within Google Cloud Shell). Public keys can be also wrapped in a self-signed X.509 certificate.

Refer to [https://cloud.google.com/iot/docs/how-tos/credentials/keys](https://cloud.google.com/iot/docs/how-tos/credentials/keys)

A. Section **Generating an RS256 key**

This section explains how to generate an RSA256 key pair (optional with X.509 certificate).

B. Section **Generating an ES256 key**

This section explains how to generate an EC256 key pair (optional with X.509 certificate).

### Add a public key to the device
Refer to [https://cloud.google.com/iot/docs/quickstart](https://cloud.google.com/iot/docs/quickstart) - section **Add a public key to the device**

1. Open previously generated public key `ec_public.pem` or `rsa_cert.pem`.
2. Copy the contents to the clipboard. Make sure to include the lines that say:<br>
   `-----BEGIN PUBLIC KEY-----` and `-----END PUBLIC KEY-----`<br>
   or<br>
   `-----BEGIN CERTIFICATE-----` and `-----END CERTIFICATE-----`.
3. On the **Device details** page for the device you created before, click **Add public key**.
4. Select the **Public key format** based on the key generated.
5. Paste your public key in the **Public key value** box.
6. Click **Add**.

Added key appears on the Device details page for your device.


In &micro;Vision
----------------

Follow these steps to create a simple application that connects to the Google IoT Cloud.

### Create project and select software components
1. Create a new project. Select the device that you are using on your development board.
2. The ![Manage RTE](./static/icons/rte.png) Manage Run-Time Environment window opens. Select the following software components:
    * **IoT Client:Google**
    * **IoT Client:MQTTClient-C**
    * **IoT Utility:MQTTPacket**
    * **IoT Utility:Socket:MDK Network**
    * **CMSIS:RTOS2:Keil RTX5**
    * **CMSIS:CORE**
    * **Security:mbed TLS**
    * **Network:** make sure to select the variant **MDK-Pro**
    * **Network:Socket:BSD**
    * **Network:Socket:TCP**
    * **Network:Socket:UDP**
    * **Network:Service:DNS Client**
    * **Network:Service:SNTP Client**
    * **Network:Interface:ETH**
    * **Network:CORE:IPv4 Release**
    * **CMSIS Driver:Ethernet/MAC/PHY (API)** (depending on your hardware)
    * **Device:Startup** (and any other Device dependent components)
    
    Optional (for `printf` debug output on the console):
    * **Compiler:I/O:STDOUT:ITM**

### Configure the software components
1.  Configure mbedTLS: **Security:mbedTLS_config.h**
    * In the Project window, double-click this file to open it. It contains generic settings for mbed TLS and its configuration requires a thorough understanding of SSL/TLS. We have prepared an example file that contains all required settings for Google IoT Cloud. The file available in `<INSTALL_FOLDER>/ARM/Pack/MDK-Packs/Google_IoT_Device/_version_/config/mbedTLS_config.h`. Copy its contents and replace everything in the project's mbedTLS_config.h file.
2.  Configure Network:: **Network:Net_Config_UDP.h**
    * Increase the **Number of UDP Sockets** to at least 6.<br>
3.  If you are using the software components described above, you do not need to configure other Network components. The default settings will work. If you do not have DHCP available in your network, please refer to the [MDK-Middleware documentation](http://www.keil.com/pack/doc/mw/Network/html/index.html) on how to set a static IP address.
4.  Configure RTX5: **CMSIS:RTX_Config.h**
    * If you are using the provided template (see below), you need to set the **System - Global Dynamic Memory size** to at least 10240:<br>
    ![RTX_Config.h](./static/images/rtx_config_h.png)<br>
    This large amount of dynamic memory is not required for custom projects.
5.  Configure Heap: **startup_\<device>.s**
    * Configure at least 48 KB (0xC000) of heap memory.<br> 
    ![Heap](./static/images/heap.png)<br>
    This is required for the mbed TLS certificates.
6.  Configure device specific hardware:
    * Configure the CMSIS Ethernet driver and other device specific components (clock system, I/O, ...) as required. Please consult your device's/board's documentation for more information.

### Add Template code
The **Google_IoT_Device** pack includes a code templates that can be used to test the connection to Google IoT Cloud. In the Project window, right-click "Source Group 1" and select "Add New Item to Group" - "User Code Template", to add user code templates.

1.  Demo sample code:
    * Add **IoT Client:Google:Simple Demo**<br>
    ![Heap](./static/images/uct.png)<br>
    * Update the following definitions as created in the Google IoT Console
      * `PROJECT_ID`: Enter the **Project ID** here
      * `CLOUD_REGION`: Enter the **Cloud region** here
      * `REGISTRY_ID`: Enter the **Registry ID** here
      * `DEVICE_ID`: Enter the **Device ID** here
2.  Device Private Key:
    * Add **IoT Client:Google:Device Private Key**. This adds the file `pkey.h` to the project.
    * Update `PrivateKey` with previously generated Device Private Key. Make sure to include the lines that say:<br>
    `-----BEGIN EC PRIVATE KEY-----` and `-----END EC PRIVATE KEY-----` for EC256 key<br>
    or<br>
    `-----BEGIN RSA PRIVATE KEY-----` and `-----END RSA PRIVATE KEY-----` for RSA256 key.<br>
3.  Add **CMSIS:RTOS2:Keil RTX5:main** and update:
    * Add an attribute for an increased stack size for the application thread. Sample requires a thread stack size of 8 KB:<br>
      `const osThreadAttr_t app_main_attr = { .stack_size = 8192U };`
    * Change application thread creation (include the attribute defined above):<br>
      `osThreadNew(app_main, NULL, &app_main_attr);`
    * Add an include for the Network library (beginning of file):<br>
      `#include "rl_net.h"`
    * Add a prototype for the example's top level function:<br>
      `extern int MQTT_Demo (void);`
    * Add Network initialization in the `app_main` function:<br>
      `uint32_t addr;`<br>
      `netInitialize();`<br>
      `do {`<br>
      &nbsp;&nbsp;`osDelay(500U);`<br>
      &nbsp;&nbsp;`netIF_GetOption(NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, (uint8_t *)&addr, sizeof (addr));`<br>
      `} while (addr == 0U);`
    * Add a call to the example's top level function into the `app_main` function after Network initialization (replace the `for` loop):<br>
      `MQTT_Demo();`
4.  MQTT Certificates:
    * Add **IoT Client:MQTTClient-C:Certificates**. This adds the file `certificates.h` to the project. This file needs careful editing. The next step explains what to add here. 
    
### Adding your certificates
Previously, you have created a template with certificates and keys for your thing. You need to provide the root CA certificate for the server (client certificate and key are not used for Google IoT Cloud).

1. Use the following `CA_Cert` (GlobalSign Root CA - R2)<br>
    `"-----BEGIN CERTIFICATE-----\n"`<br>
    `"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G"`<br>
    `"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp"`<br>
    `"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1"`<br>
    `"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG"`<br>
    `"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI"`<br>
    `"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL"`<br>
    `"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8"`<br>
    `"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq"`<br>
    `"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd"`<br>
    `"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa"`<br>
    `"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB"`<br>
    `"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH"`<br>
    `"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n"`<br>
    `"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG"`<br>
    `"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs"`<br>
    `"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO"`<br>
    `"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS"`<br>
    `"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd"`<br>
    `"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7"`<br>
    `"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg=="`<br>
    `"\n"`<br>
    `"-----END CERTIFICATE-----\n";`<br>
2. Leave `ClientCert` unchanged (not used)<br>
3. Leave `ClientKey` unchanged (not used)<br>

### Optional: Configure debug
The Demo sample code is prepared to output `printf` statements for debugging purposes. To show these effectively, you can use the software component **Compiler:I/O:STDOUT:ITM** to show the messages in the [Debug printf](http://www.keil.com/support/man/docs/uv4/uv4_db_dbg_serialwin.htm) window. To enable this output, do the following:

1.  Open the ![Options for target](./static/icons/oft.png) Options for target dialog (**Alt+F7**).
2.  Go to the **C/C++** tab and enable mbedTLS debug messages by adding the following global define to the C pre-processor:<br>
    `MBEDTLS_MSG_INFO=1`<br>
3.  Go to the **Debug** tab. Select the debug adapter that is connected to your target and configure these **Settings**:
    * **Debug** tab: Set **Port** to **SW**.
    * **Trace** tab: Set the **Core Clock** to the system clock of your target, Select **Trace Enable** and set make sure that **ITM Stimulus Port 0** is enabled:<br>
![Debug Setup](./static/images/debug_setup.png)

### Run/debug the application
1.  ![Build target](./static/icons/build.png) Build the application (**F7**) and ![Download to target](./static/icons/Flash.png) download (**F8**) to target.
2.  Enter ![Start/Stop Debug Session](./static/icons/debug.png) debug (**CTRL+F5**) and ![Run](./static/icons/run.png) run (**F5**) the application on the target and monitor the console output via ![Debug (printf) Viewer](./static/icons/uart_window.png) **Serial Window - Debug (printf) Viewer**. You should see something similar:
    ```
    . Seeding the random number generator... ok
    . Loading the CA root certificate ... ok (0 skipped)
    . Connecting to mqtt.googleapis.com/8883... ok
    . Setting up the SSL/TLS structure... ok
    . Performing the SSL/TLS handshake... ok
      [ Protocol is TLSv1.2 ]
      [ Ciphersuite is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256 ]
      [ Record expansion is 29 ]
    . Verifying peer X.509 certificate... ok
    MQTT Connected
    MQTT Publish Device State (active)
    Configuration Message arrived:
    MQTT Publish Event (1)
    MQTT Publish Event (2)
    MQTT Publish Event (3)
    MQTT Publish Event (4)
    MQTT Publish Event (5)
    MQTT Publish Event (6)
    MQTT Publish Event (7)
    MQTT Publish Event (8)
    MQTT Publish Event (9)
    MQTT Publish Event (10)
    MQTT Publish Device State (idle)
    . Closing the connection... done
    MQTT Disconnected
    ```
