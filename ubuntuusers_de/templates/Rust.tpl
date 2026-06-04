## Macro=[[Vorlage(Rust)]]

{{|<title="Hinweis:" class="box notice">
<@ if $arguments.0 == "" @>Dieser Artikel beschreibt die alte Implementation des Programms
<@ if $arguments.1 != "" @> aus dem Paket [packages:<@ $arguments.1 @>:] {en}<@ endif @>.

<@ if $arguments.2 != "" @>
<@ if $arguments.3 != "" @>Ab Ubuntu <@ $arguments.3 @><@ else @>Inzwischen<@ endif @> ist auch mit Paket [packages:<@ $arguments.2 @>:] {en} eine neue Implementierung in der Programmiersprache Rust für Ubuntu verfügbar
<@ if $arguments.4 != "" @>, und ab Ubuntu Version <@ $arguments.4 @> wird die Implementierung in Rust als Standard bevorzugt<@ endif @>.

Die Implementierung in Rust strebt Funktionsgleichheit mit der alten Implementierung an.
<@ if $arguments.5 == "" @> Es können aber kleine Abweichungen auftreten.
<@ else @> Es wurden bereits kleine Abweichungen bekannt. Zur Zeit ist nicht vorhersehbar, ob diese Abweichungen bestehen bleiben.
<@ endif @> Dies bedeutet, dass das hier Beschriebene von der am eigenen Rechner vorgefundenen Situation abweichen kann.
<@ endif @>
<@ else @>Dieser Artikel beschreibt die neue Implementierung in Rust des Programms
<@ if $arguments.2 != "" @> aus dem Paket [packages:<@ $arguments.2 @>:] {en}<@ endif @>.
<@ if $arguments.4 != "" @> Diese Version wird bei Ubuntu ab <@ $arguments.4 @> als Standard bevorzugt<@ endif @>.
<@ endif @>
|}}
