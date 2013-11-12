final KnownHosts databaseKnownHosts = new KnownHosts();
databaseKnownHosts.addHostkeys(knownhostsFile);	
connection = new Connection(ipaddress);
connection.connect(new SimpleVerifier(ActivityRemoteTerminal.this, databaseKnownHosts));
