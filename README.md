<table border="0">
  <tr>
    <td align="left" valign="middle">
    <h1>EFR32 Bluetooth Mesh Application Examples</h1>
  </td>
  <td align="left" valign="middle">
    <a href="https://www.silabs.com/wireless/bluetooth">
      <img src="http://pages.silabs.com/rs/634-SLU-379/images/WGX-transparent.png"  title="Silicon Labs Gecko and Wireless Gecko MCUs" alt="EFR32 32-bit Wireless Microcontrollers" width="250"/>
    </a>
  </td>
  </tr>
</table>

# Silicon Labs Bluetooth Mesh Applications #

The Silicon Labs Bluetooth Mesh stack allows for a wide variety applications to be built on its foundation. This repo showcases some example applications built using the Silicon Labs Bluetooth Mesh stack.

This repository provides both SLCP projects (as External Repositories) and SLS projects as standalone projects, which are configured for development boards.

## Examples ##

- Room Monitor
- Temperature Log
- EnOcean Switch Proxy Server

## Requirements ##


1. Silicon Labs EFR32 Development Kit
2. Simplicity Studio 5
3. Gecko SDK Suite 4.3.1, available via Simplicity Studio or [here](https://github.com/SiliconLabs/gecko_sdk)
4. Third-Party Hardware Drivers extension, available [here](https://github.com/SiliconLabs/third_party_hw_drivers_extension)

## Working with Projects ##

1. To add an external repository, perform the following steps.

    - From Simpilicity Studio 5, go to **Preferences > Simplicity Studio > External Repos**. Here you can add the repo `https://github.com/SiliconLabs/bluetooth_mesh_applications.git`. 

    - Cloning and then selecting the branch, tag, or commit to add. The default branch is Master. This repo cloned to `<path_to_the_SimplicityStudio_v5>\developer\repos\`

2. From Launcher, select your device from the "Debug Adapters" on the left before creating a project. Then click the **EXAMPLE PROJECTS & DEMOS** tab -> check **bluetooth_mesh_applications** under **Provider** to show a list of Bluetooth example projects compatible with the selected device. Click CREATE on a project to generate a new application from the selected template.

## Legacy Projects - Importing *.sls projects ###

1. Place the *.sls file(s) to be imported in a folder.

2. From Simpilicity Studio 5, select **File > Import**, select the folder containing *.sls file(s). Select a project from the detected projects list and click on Next. Name the project and click Finish.

See [Import and Export](https://docs.silabs.com/simplicity-studio-5-users-guide/5.6.0/ss-5-users-guide-about-the-simplicity-ide/import-and-export) for more information.

## Porting to Another Board ##

To change the target board, navigate to Project -> Properties -> C/C++ Build -> Board/Part/SDK. Start typing in the Boards search box and locate the desired development board, then click Apply to change the project settings. Ensure that the board specifics include paths, found in Project -> Properties -> C/C++ General -> Paths and Symbols, correctly match the target board.

## Documentation ##

Official documentation can be found in our [Developer Documentation PDF](https://www.silabs.com/documents/public/reference-manuals/bluetooth-le-and-mesh-software-api-reference-manual.pdf).

## Reporting Bugs/Issues and Posting Questions and Comments ##

To report bugs in the Application Examples projects, please create a new "Issue" in the "Issues" section of this repo. Please reference the board, project, and source files associated with the bug, and reference line numbers. If you are proposing a fix, also include information on the proposed fix. Since these examples are provided as-is, there is no guarantee that these examples will be updated to fix these issues.

Questions and comments related to these examples should be made by creating a new "Issue" in the "Issues" section of this repo.

## Disclaimer ##

The Gecko SDK suite supports development with Silicon Labs IoT SoC and module devices. Unless otherwise specified in the specific directory, all examples are considered to be EXPERIMENTAL QUALITY which implies that the code provided in the repos has not been formally tested and is provided as-is.  It is not suitable for production environments.  In addition, this code will not be maintained and there may be no bug maintenance planned for these resources. Silicon Labs may update projects from time to time.
