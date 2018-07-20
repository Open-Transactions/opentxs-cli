// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

-----BEGIN SIGNED SMARTCONTRACT-----
Hash: HASH256

<smartContract
 activatorAcctID=""
 activatorNymID=""
 canceled="false"
 cancelerNymID=""
 creationDate="2015-06-25T05:29:08"
 lastRecipientAcctID=""
 lastRecipientNymID=""
 lastSenderAcctID=""
 lastSenderNymID=""
 nextProcessDate="1970-01-01T00:00:00"
 notaryID=""
 transactionNum="0"
 validFrom="2015-06-25T05:29:08"
 validTo="1970-01-01T00:00:00"
 version="2.0">
<scriptableContract
 numBylaws="1"
 numParties="3"
 specifyInstrumentDefinitionID="false"
 specifyParties="false">
<party
 authorizingAgent="agent_alice"
 name="party_alice"
 numAccounts="1"
 numAgents="1"
 openingTransNo="0"
 ownerID=""
 ownerType="nym"
 signedCopyProvided="false">
<agent
 doesAgentRepresentHimself="true"
 groupName=""
 isAgentAnIndividual="true"
 name="agent_alice"
 nymID=""
 roleID="" />
<assetAccount
 acctID=""
 agentName=""
 closingTransNo="0"
 instrumentDefinitionID=""
 name="acct_alice" />

</party>
<party
 authorizingAgent="agent_bob"
 name="party_bob"
 numAccounts="1"
 numAgents="1"
 openingTransNo="0"
 ownerID=""
 ownerType="nym"
 signedCopyProvided="false">
<agent
 doesAgentRepresentHimself="true"
 groupName=""
 isAgentAnIndividual="true"
 name="agent_bob"
 nymID=""
 roleID="" />
<assetAccount
 acctID=""
 agentName=""
 closingTransNo="0"
 instrumentDefinitionID=""
 name="acct_bob" />

</party>
<party
 authorizingAgent="agent_judy"
 name="party_judy"
 numAccounts="1"
 numAgents="1"
 openingTransNo="0"
 ownerID=""
 ownerType="nym"
 signedCopyProvided="false">
<agent
 doesAgentRepresentHimself="true"
 groupName=""
 isAgentAnIndividual="true"
 name="agent_judy"
 nymID=""
 roleID="" />
<assetAccount
 acctID=""
 agentName=""
 closingTransNo="0"
 instrumentDefinitionID=""
 name="acct_judy" />

</party>
<bylaw
 language="chai"
 name="main_bylaws"
 numCallbacks="2"
 numClauses="7"
 numHooks="2"
 numVariables="12">
<variable
 access="important"
 name="bool_alice_dispute"
 type="bool"
 value="false" />
<variable
 access="important"
 name="bool_bob_reply"
 type="bool"
 value="false" />
<variable
 access="important"
 name="bool_judy_judgment"
 type="bool"
 value="false" />
<variable
 access="important"
 name="nState"
 type="integer"
 value="0" />
<variable
 access="constant"
 name="str_alice_amount"
 type="string"
 value="exists">
eNozNDUFAAE1AJw=

</variable>
<variable
 access="important"
 name="str_alice_dispute"
 type="string"
 value="exists">
eNrzTFNwzMlMTlVIySwuKC1JLdZRyEgtgvEUclOLixPTUxXS81OLQRKpegC1dRGt

</variable>
<variable
 access="important"
 name="str_bob_reply"
 type="string"
 value="exists">
eNpzyk9SL1YoSi3IqVQoyVdwzMlMTgUKpGQWF5SWpCqk56cWK2SkFqXqAQAsIw5z

</variable>
<variable
 access="constant"
 name="str_consideration"
 type="string"
 value="exists">
eNrTcM7PK85MSS1KLMnMz1PILS0uUUhKVUhJLU4uykxKTVHISC1K1dMEACJ9Dis=

</variable>
<variable
 access="important"
 name="str_error_status"
 type="string"
 value="exists">
eNrTcC0qyi9SyE0tLk5MTy1WSM9XyEgtStXTBAB0hAi8

</variable>
<variable
 access="constant"
 name="str_judy_cut"
 type="string"
 value="exists">
eNozBQAANgA2

</variable>
<variable
 access="important"
 name="str_judy_judgment"
 type="string"
 value="exists">
eNrzKk2pVC9WSMvMS8xRyCpNSc9NzStRSM9PLVbISC1K1QMAwF4Lnw==

</variable>
<variable
 access="constant"
 name="str_judy_render_cut"
 type="string"
 value="exists">
eNozMgAAAJYAYw==

</variable>
<clause
 name="alice_dispute_clause">
eNptkUFr20AQhc/Wr3j4EhkSx4GQlgQfnDSHQmlC7J7FWhpbC9KumBkhTOl/z6wU
95AWdi8zs+99+6ZjHzSfb5/evr/unr/h6cfm1/b5Hhvee2WnVIGk5DjcY1d7gZ1N
40u6EFReul4JZeN6oSXylwAHaR1rGYM9LnW5mC8yZPAH5Cus1whbNc0FgOtrTJXI
rWsgqY58qH1ZYyC0vSj2BB8u7SJyRYxD5MkdGs/2SzP4PctmkzLWuHlI2tNY7f5i
Vgb4Mw5J2zF9+N3g6gqD8+rDcVR/jHswdc3JZGf7GJvCJaHi/Nc1lHv65BAPB196
1zQn6/rjkdhSq433/Cp26mMwgF3NsT/WsVfY+AQhl9CULFPrfBAIdS4Fb2pMZfp4
lWBE+R+WNNgW1jH8h1mC2tU0VUnNv3MihmL5/WdtopFPy8yUSQsxp1BJ0Qf1TaG+
Jc7nd6vbr6vVfDEu6wsqdxKMA2NM44amrDL8ybJ3nt7C/A==

</clause>
<clause
 name="bob_reply_clause">
eNptUU1r4zAQPce/4pFLEyhuWpZtackh/Ti0dJdl456NbE1itbZkpBEhLPvfd2Qt
gULB+DB6875m9MbyYr59+P38q3p6xMPr5m37dIuNbwx7xaRBofXucIuqMwHy3bvm
LKDtVQyEnfPw1Dqvjd0jITyN/RHssOlNS4LUJoyRqZwvCxQwOywusV7Dblnol8DF
BabBhEenThv6DgdlOBEnmaw70Zf48bat0BCMBSfVkMjKAn9mxSwzY42ru4ldFifa
tGpIl/jpDidmZ/ES9Z7Sf7LtyWryUHiX8UCWy2LWONfXjWvqHG4N9pE+k7vdzrRG
9fL8X+erDrCoOu/ivnORIeDsO5znEJ4GZayEoVGl7ieu1K2YXhazwP6TiQQaaplK
jmym6ihPiSXCqEIQG1LR6WrBaBKrIkci7fyxLISXuA6iY3Woo2XT12wG8ov599W3
m9Vqnm90Da2OARMglzXEwBjUB0lZPvbiQg7wt/gHk/fGIA==

</clause>
<clause
 name="judy_render_judgment">
eNqtVt9v2kgQfjZ/xZSHFuvALQlP5XJSftCGqoEqEFX3hBZ7gb0zNtpdB6E0/3u/
2bXB0ESVTocIitez33w7883MNjZaZbbVnFzfD79NBzd0/fXyYTL4SJd6rqwWViYk
Tazz7UearpQhfL8UyVLy7+6doTgVhZG0yDVpmSUScEv6BxZrmVlSGdmVJPnID/mC
BCXKbAoro2bYoAapBbXO6OKCsomFr5Dwef+eurx0mapY0kqYak/Spqt87la03KRK
Jn3aCmXZI/s/8KIs35LNS0bwWhGK6O5hMqW59MxwGMN+owY9NYJHoSn7pLKkG1zQ
RmixnhnL54kWWGw1HaFm2K9Znr1sKfaWNdvzl21xpGPM3st2c29HlWVeZIljBPNO
t19fBmS12AgQzs5//7j9Rtq6QxJZcvDEcY7zzIoy12m+lcZSKpfIzBrBxUosrMoz
hwURsJUp5v505YICd7GWhlolDGdMQl8uEiEhvZ0uyyXLLZIN1xHDNQJWkE8a/XVB
H0J6OqJ6UWa0T8/e1huflcZv31K18Gd9X/gKzpnDqXk9P/HKASltz0989k599vY+
sSt8EaPnMP6HFF4Nvo6/0/R2OKFv4+Fo2naxrB0QpdDptn1ufGK2wmTvymCTsCTS
NKLh4kDSAe/3weI3G4G9I6ElfeB0LrWEOLRz6VNu1rBEueaZrKB5eauyjO3mMnat
RtnqDQwPWiGub6jM0EJpY0t5AGZ6O6DR5d2A7gaj6XA8QpP7NLyfIARXf7uXXx5u
Pg/ahNDw0/fhaDS4b9Pl9fX4/mY4+kzTsQ/c9fhm8OaNx4RXLdfQPPeXUtOVZBMZ
KwMmFIsMfSZOi4SthckzhKdN0sYR2hSKZC3+5VLQ7hgO2B8WIFwNVPWnBIdHh5Nl
gRmpo0YlwkMKvbwawRP+At9QoaPzfhAwsuuLrpuCXgJIbpl+4yY3Rs3TXRQhas9B
HZm16HAdxi1q+xHO2Q4N88TuuIIAEzzxT0kFHanXZxDs3nMRsS2Q9N0RKWABP4Dw
n3FKiGqGyO5mVQ8/aY/9Kh9uUUJS+M8YYKGT+Gm1qyUlotZY04qngrVyvbGsUFPE
qygsu+We3KGF1Qi2mKFUlhF89ErGods85hdbZWSbkpyb1YorotNBqneYOgY818rY
3UbWG1jrvD4Df/ygVq+2EPpgz/M8/SUSVhfyNMH5YqFi5cLqByBo7wcgH9KPCraf
5lakAXD2UY4LG9l8xpeCkP44rHuko9c+8oxAYo3Euwlfhnsjdu1S+xwAkQBE+TFA
CylNFFQ84OHABAfKy7y2DgQx9krjOW4GVGQoCrOaLaA10xJxbB3FNvm5O3Nvm+0j
ZMQwKAVbjzWfwD2/Xh0sRQ6/93/2sn/n+JSAk65f8fEJvaoDmXIbWxxnman0XqVS
1sRvieCW8AuNV3ngm0iUoHoEgRkPcY2nVsiF50h64fF2qXWuGdEWBp6btbsWsrVc
OpFxTeyvfv5iiLZdoL5irVBpGAzgLOap5EtDwrW6VujhAsYSh/LNj+aCqxdCcVWa
mS06TpM5YRY2Gj8BljhNKA==

</clause>
<clause
 name="on_activate">
eNqVUk2P1DAMPbe/wvRCKpbuaBEHFnEowyIhIS1ihnPlSTw7gTRZxe6iCvHfcT8W
GIkLVS+xn9978ct99lFMtdt+/vBpf/MOth/bL7uba2jzwUtGIQfENqfv17A/eQb9
5URgAw5McEx5Pt7G1op/ULip4ZTStwbMbQQE7jGLTVGprDR1VZdFcXmpVATYpyEK
tMFbmmiZovPxDiTB23QoFmC7gJgEkL1bFNF9HZy3KD5FOBKxqsUUn2c6DtHhIVBT
w3/O/z1bPmCGuE+CYSfIJ3LFGzAsucPJbLc4byR10+5qeAZTTznHzg7/rme9HOWz
dv161lHIHyXV0baWdBHmzIGiYcYf1sK8N8UrgZ67yT0btFYWkxdQLWbndnVxrlMr
mT+CeXLGpkv7URaTZ8o55WlUBjbVe/RB34Hkcc0H17DXpwG/AwazTUNw8anAna58
NjWnDvo5epzrHgdMXf4sKehL0qx2g7XEvFxoUlrGZ08kHZNOOe509T504nvKprp6
+epqs9lU9RT2iw04HBlmBNzj2JNmj4OkXrO2GMKoVWbi9ZE1ql7+AnrQBmI=

</clause>
<clause
 name="on_process">
eNq1lE1v2zAMhs/2r2BzqY1l7todBrTIIet66DCgRZNhR0OWmUSrLQWS3CAY9t9H
Svlql7T7BHKI9cH3JflQc6u0z3qjy7vr2/HVB7j8NPw8ujqHoa2Ut8JjDeikNYtz
GM+UA/r5GYJsROcQJsaGzxt9a41E57IcZsbcF5DdaBDgWmG9NJoiSV/kvTyFFNQE
sjcwGIAeeRLI4eQE4rexrWjA8WofWqNxCVODpGjgval2VYoUvqUJXRx1klcKuPbH
DipElqWrflbArViGe0LX8LGrl/3wr0byoh5II2aydpcmztsSrTW2ZAudgwH0VvGP
9sXPSEDp6XMavP1IJe9dpGnyICxUpxS/0yTlZuWk07XLhJS+/Bqi9ESjJJZht9cH
tsYbpex8niScOWdHilMMujBDuy4gnYFMGxA1XVGSXBgNE0Sq0tUD2qWfsa0Fak8N
MoZSyVeWzvZbqky111FcEa3pNLuKptYV4bQjOhBPFCuRt/tFQrDDiVvUNdqYPwnd
IV9lLdqdtpwLZUjnlZYIT5NfCAcauZHCLslHmmwpKNe9yYjO7ylgQ2Azo6c7jLJk
+P6CFEzxWDD7lCdRYXHeUMsqKrumbHlM3kEtlg6ERejmR2tYs1Fwx9WpVa2PfbwK
SoNXLbkfcu6wUJpQWxJFE9E1nruzD00Oo5EaGqIorCN7awsckceV9Dekck/iXFVC
3vNcDWPVGypN4IjS0Ua/tqG8omqQy7pDrR4bL5oRdwfrZADZUw4Kb0p+UnJ4ta95
m+38IgakM9uQFJD2aYnMZo+k8n8wNn84NHny/HQcBHfrP9+Zjdjjn6ajD/Omi4/r
fyP67AnR4Zsx2gDEDJMFvVukiVAN8U6wtOIe6fmrUSrHJlbcbviOeIdL+/kOoamd
UNOzzvtT9DAXqgbhQTRNEcJcazCWeGFJgh5IaW6NR7l9TjvnTYu2v0M0oe64aI/I
PvCqB4vr2WE0Kb9tXi/OS5iH5G8GYncSXhqU35uTF8bkl2A9iFPCPP0Ayb/GXA==

</clause>
<clause
 name="party_may_cancel_contract">
eNorKMrMK9FQ8nWMVHB29HN29bFX0Egrys9VyE2sjE9OzEtOzYlPzs8rKUpMLlFI
TszJSUpMztZTCEotKS3Ky8xLV0hLzClOVQBrKU4uyiwo0dNU0rTm4gKLW3MBAEC2
IJc=

</clause>
<clause
 name="party_may_execute_clause">
eNqVzcEKgkAQgOG7TzF40N1LLyASER67FdRpGdcxrN11WXchyd49zcKb0GVgmJ9v
rGuMZ/Fhd4HiXOxPx2ILrHatBo29oAfJ4ElIhaEjkKhUifK+4THPoqipgQFjFh1q
MU7fC4OaIM8hnVdUjaSUQ5LAN5ulpfsUomo6u/xJOYdhWJfLtlx1x7twZFX/h3kL
Vb+KTsGomorcFF81GT/BHJ7gyAdnwLtAGbx+a42qoyx6A7H1eeo=

</clause>
<hook
 clause="on_activate"
 name="cron_activate" />
<hook
 clause="on_process"
 name="cron_process" />
<callback
 clause="party_may_cancel_contract"
 name="callback_party_may_cancel_contract" />
<callback
 clause="party_may_execute_clause"
 name="callback_party_may_execute_clause" />

</bylaw>

</scriptableContract>
<accountList
 count="0"
 type="stash" />

</smartContract>
-----BEGIN SMARTCONTRACT SIGNATURE-----
Version: Open Transactions 0.94.0-562-ga35c82f
Comment: http://github.com/FellowTraveler/Open-Transactions/wiki
Meta:    SFUK

KqWhqnnCEFhZdjl8EYjL+sL5fyRRD1cC8WwFug02E7viWcFI6/gP9SQ06iOOmSNG
yJgn7XCbicihg80U4Qk1SOLwUMC8oL9xMvYZ94aXassXJAV1tqcMde7/KmpnPIbJ
qzkGX86ykoaZbWNjpYeHb6CMdjWw8SKsHw36ldePQH4=
-----END SMARTCONTRACT SIGNATURE-----

-----BEGIN SMARTCONTRACT SIGNATURE-----
Version: Open Transactions 0.94.0-562-ga35c82f
Comment: http://github.com/FellowTraveler/Open-Transactions/wiki
Meta:    SGGP

w36oZpidM3eitC1JmSnBAJm/ptbObt8+V62lc8wlpOt+yrlEYFk9ft5YPJCG0Ebx
gllJFc5skdq7r4Xh04ermAESpHeiV6e4jMlVhcgBfm5NKPF83WAFVJyscq9kkaD9
nyo95BvhcrDKUKM/4dt+iNLojRNKdbFTiA6teCqnBbU=
-----END SMARTCONTRACT SIGNATURE-----

-----BEGIN SMARTCONTRACT SIGNATURE-----
Version: Open Transactions 0.94.0-562-ga35c82f
Comment: http://github.com/FellowTraveler/Open-Transactions/wiki
Meta:    SGGP

prj/kSYjoNWFYvrOFcHjZdj6yA7AAJrr2QCe2SIelrPP186/XxqOZrXuZ5fP6yI8
K8uTLBgB7P9+ef8KWeS5s6ad2fxa71Mjp7U31zPZ3+a1x45LN0jAbsAokm+QQ5pl
q7tXea5nINQ/zzSUMc3TwkHDcP8EV38+nKQuImY+ip8=
-----END SMARTCONTRACT SIGNATURE-----
