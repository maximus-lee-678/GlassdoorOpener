⛏ GlassdoorOpener
==============
| C Glassdoor Review Crawler developed for the `HadoopWSDEG <https://github.com/maximus-lee-678/HadoopWSDEG>`_ Project.
| Automatically grabs up to 1000 reviews for a specified number of companies.
| Get the latest release `here <https://github.com/maximus-lee-678/GlassdoorOpener/releases>`_.
| ℹ Anti-viruses may flag this executable as a virus, which may cause it to not run properly or be deleted.

⛓ PythonFuser
--------------
Formats TSVs provided by GlassdoorOpener.

* Sanitises Input
* Handles empty fields
* Removes duplicated data
* Combines all TSVs into a single one

👥 Team Members
----------------
.. list-table::
   :header-rows: 1

   * - Name
     - Creator Of
   * - `Maximus Lee <https://github.com/maximus-lee-678>`_
     - GlassdoorOpener
   * - `Aloysius Woo <https://github.com/AloysiusWooRY>`_
     - PythonFuser

🚓 Roadmap
-----------

* Selectable minimum star ratings
* Selectable maximum review count
* Multithreading support
* Checkpointing
* Storage in RAM instead of Disk

🖥️ VS2022 Setup
---------------

This program was developed in VS2022 using libcurl for Windows.
libcurl was installed using `vcpkg <https://github.com/microsoft/vcpkg>`_.

1. Get `latest <https://github.com/microsoft/vcpkg/releases>`_ vcpkg zip file and extract it.
2. Open **Developer Command Prompt for VS** and cd to wherever vckpg has been extracted to.
3. Run:

.. code-block:: console

  bootstrap-vcpkg.bat

4. Run:

.. code-block:: console

  vcpkg.exe integrate install

5. Run:

.. code-block:: console

  vcpkg.exe install curl

OR

.. code-block:: console

  vcpkg.exe install curl --triplet=x64

6. After installation, in the project: ensure solution configuration (next to debug buttons) is targeting right triplet (x86 or x64).
