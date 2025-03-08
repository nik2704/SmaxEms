
# SMAX Client CLI

This is a command-line interface (CLI) for interacting with the **SMAX** system's REST API. The project allows performing various operations such as retrieving records, uploading attachments, and handling JSON actions. It supports a flexible configuration system and customizable output formats.

## Table of Contents
- [Features](#features)
- [Project Structure](#project-structure)
- [Input Parameters](#input-parameters)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [License](#license)

## Features
- **Use of EMS API**: Retrieve records from various tables.
- **Define Layouts**: Choose the fields to retrieve.
- **Filters**: Specify conditions to filter the records.
- **Create/Update records**: Records may be created / updated using CSV file
- **Attachment Management**: Retrieve information about attachments, download, and save them to disk.
- **JSON Handling**: Get values of JSON fields like Task Plans or attachments and save them to files or display them on the console.

## Project Structure
```plaintext
/main.cpp
/utils
    utils.h
    utils.cpp
/SmaxClient
    SMAXClient.h
    SMAXClient.cpp
    ResponseHelper.h
    ResponseHelper.cpp
    ConsoleSpinner.h
    ConsoleSpinner.cpp
    ConnectionProperties.h
    ConnectionProperties.cpp
/RestClient
    RestClient.h
    RestClient.cpp    
/Parser
    Parser.h
    Parser.cpp
```

## Input Parameters

### Options
- `--action`: Action to perform (GET, CREATE, UPDATE, JSON, GETATTACHMENTS). Default is `GET`.
- `--config-file`: Full path to the configuration file (INI format).
- `--csv`: CSV file for CREATE or UPDATE actions.
- `--output-folder`: Folder to store output data. Default is `output`.
- `--entity`: Entity name (e.g., `Request`).
- `--filter`: Filter condition (e.g., `Id='52641'`).
- `--layout`: Layout of fields to be retrieved (comma-separated).
- `--password`: Password for authentication.
- `--smax-protocol`: Protocol for the API (`http` or `https`). Default is `https`.
- `--smax-host`: Fully qualified domain name (FQDN) of the SMAX server.
- `--smax-port`: Connection port for EMS and bulk requests. Default is `80`.
- `--smax-secure-port`: Secure HTTPS port. Default is `443`.
- `--tenant`: Tenant ID.
- `--username`: Username for authentication.
- `--verbose`: Enable verbose output.
- `--json-action-field`: JSON action fields.
- `--json-action-output`: Output method for JSON actions (`file` or `console`). Default is `console`.
- `--json-action-output-folder`: Folder for JSON action output.
- `--att-action-output`: Output method for attachment actions (`file` or `console`). Default is `console`.
- `--att-action-field`: Field for attachment actions.
- `--att-action-output-folder`: Folder for attachment output.

## Usage

### Command help
```bash
smax_ems -h
Options:
  --action arg (=GET)                    Action (GET is default, CREATE, UPDATE, JSON)
  --config-file arg                      Full name of a config file
  --csv arg                              CSV file name (required if action is not GET)
  --output-folder arg (=output)          Root output folder
  -e [ --entity ] arg (=Request)         Entity name
  --filter arg                           Filter (like "Id='52641'")
  -l [ --layout ] arg (=Id,DisplayLabel) Layout (fields)
  -P [ --password ] arg                  Password
  -p [ --smax-protocol ] arg (=https)    Protocol (http | https default)
  -s [ --smax-host ] arg                 FQDN of SMAX server
  -c [ --smax-port ] arg (=80)           Connection port (80 is default)
  -z [ --smax-secure-port ] arg (=443)   HTTPS port (443 is default)
  -t [ --tenant ] arg                    Tenant ID
  -U [ --username ] arg                  Username
  -v [ --verbose ]                       Enable verbose output
  --json-action-field arg                Json action field
  --json-action-output arg (=console)    Json action output
  --json-action-output-folder arg        Json action output folder
  --att-action-output arg (=console)     Json action output
  --att_action_field arg                 Field with attachments
  --att-action-output-folder arg         Attachments action output folder
  -h [ --help ]                          Help
```
### Example Command
```bash
smax_ems --config-file=config.ini -U USER_NAME -P PASSWORD
```

### Example Command (prints steps and URLs with JSON body only, WITHOUT execution)
```bash
smax_ems --config-file=config.ini -U USER_NAME -P PASSWORD -v
```

### Examples of INI files (--config-file option)
```bash
example1-get.ini
example2-json_fields.ini
example3-create_requests.ini
example4-get_attachment_list.ini
example5-get_attachment_files.ini
```

## CSV Example
```csv
PhaseId,OfferingWorkflow,Description,ProcessId,DisplayLabel,RequestsOffering,RequestedByPerson,RequestedForPerson
Log,6,"<p>New request</p>",Service,Request for anything,36261,24251,24251
```
### Configuration File template (config.ini)
```ini
smax-protocol=https                             # Protocol used for the API
smax-host=SMAX_FQDN                             # Hostname of SMAX server
smax-port=80                                    # Port for EMS and bulk requests
smax-secure-port=443                            # Port for getting a token
tenant=TENANT_ID                                # use real TENANT ID
entity=Request                                  # Entity name
layout=Id,RequestAttachments                    # Layout (fields are expected to be returned)
filter=Id='ID1' or Id='ID2'                     # Filter (for the GET action)
action=JSON                                     # Action (GET | CREATE | UPDATE | JSON | GETATTACHMENTS)
csv=data.csv                                    # CSV data for CREATE or UPDATE
output-folder=output                            # root output folder
json-action-field=JSON_FIELD_LIST               #like TaskPlanForApprove,TaskPlanForClassify,...
json-action-output=file                         # console or file
json-action-output-folder=json_field            # Folder
att-action-output=file                          # console of file
att_action_field=RequestAttachments             # Attachment field
att-action-output-folder=attachments            # Subfolder of output-folder
```

## Dependencies
- **Boost**: Required for program options and network communication.
- **nlohmann/json**: For JSON processing.

## Deployment
### Linux
- **get the latest release**: using the link
    https://github.com/nik2704/SmaxEms/releases/latest
- **install downloaded package (for example "package_name.deb")**:
    - using dpkg: ```sudo dpkg -i package_name.deb```
    - or using apt: ```sudo apt install ./package_name.deb```
    - or using gdebi: 
        ```
        sudo apt install gdebi-core
        sudo gdebi package_name.deb
        ```
- **Command**: use ```smax_ems``` command.

### Windows
- **configure WSL**: ```wsl --install -d Ubuntu```
- **get the latest release**: using the link
    https://github.com/nik2704/SmaxEms/releases/latest
- **install downloaded package (for example "package_name.deb")** - installation like for Linux, for example:
    - using apt: ```sudo apt install ./package_name.deb```
- **Command**: use ```smax_ems``` command.



## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.