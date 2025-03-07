
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
### Example Command
```bash
./smax-cli --config-file=config.ini --action=GET --entity=Request --filter="Id='52641'" --output-folder="output"
```

## CSV Example
```csv
PhaseId,OfferingWorkflow,Description,ProcessId,DisplayLabel,RequestsOffering,RequestedByPerson,RequestedForPerson
Log,6,"<p>New request</p>",Service,Request for anything,36261,24251,24251
```
### Configuration File Example (config.ini)
```ini
smax-protocol=https
smax-host=eu3-smax.saas.microfocus.com
smax-port=80
smax-secure-port=443
tenant=243850165
entity=Request
layout=Id,RequestAttachments
filter=Id='189421'
action=GETATTACHMENTS
csv=data.csv
output-folder=output
json-action-field=TaskPlanForApprove,TaskPlanForClassify
json-action-output=file
json-action-output-folder=json_field
att-action-output=file
att-action-field=RequestAttachments
att-action-output-folder=attachments
```

## Dependencies
- **Boost**: Required for program options and network communication.
- **nlohmann/json**: For JSON processing.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.