## Macro=[[Vorlage(PipInstallation, %%Python-Modul%%)]]
Der Python-Paketmanager [:pip:] stellt häufig aktuellere Versionen von Programmen als in den Paketquellen bereit. Folgendermaßen lässt sich das Programm darüber installieren:
{{|<class="bash">{{{pip3 install <@ $arguments @>       # Programm wird nur für den aktuellen Nutzer installiert  }}}|}}

[:pip#Ab-Ubuntu-23-04:Ab Ubuntu 23.04] muss in eine [:venv:virtuelle Umgebung für Python] installiert werden:

{{|<class="bash">{{{
python3 -m venv venv-name && source venv-name/bin/activate # venv-name durch den gewünschten Namen ersetzen
pip3 install <@ $arguments @>       # Programm wird nur für den aktuellen Nutzer installiert  
}}}
|}}

[[Vorlage(Fremd, Software)]]
