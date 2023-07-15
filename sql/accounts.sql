DROP TABLE IF EXISTS accounts;

CREATE TABLE accounts(
	account_id bigserial PRIMARY KEY,
	name text NOT NULL,
	password text NOT NULL
);