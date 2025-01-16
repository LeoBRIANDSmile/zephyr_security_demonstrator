#!/bin/bash

# Variables - Remplacez ces valeurs par les vôtres
CA_KEY="CA.key"
CA_CERT="CA.pem"
CA_SUBJECT="/CN=MyRootCA"
HOST_NAME="tintin"
HOST_KEY="host-key.pem"
HOST_CSR="host-csr.pem"
HOST_CERT="host-cert.pem"
CA_CERT_DER="CA.der"
EXT_FILE="host.ext"
DAYS_VALID_CA=3650  # Validité du certificat CA (10 ans)
DAYS_VALID_HOST=365  # Validité du certificat de l'hôte (1 an)

# Nettoyer l'environnement avant la génération
rm $CA_KEY $CA_CERT $HOST_KEY $HOST_CSR $HOST_CERT $CA_CERT_DER

# Générer la clé privée du CA
openssl genpkey -algorithm RSA -out $CA_KEY -pkeyopt rsa_keygen_bits:2048
if [ $? -ne 0 ]; then
  echo "Erreur lors de la génération de la clé privée du CA"
  exit 1
fi

# Générer le certificat du CA
openssl req -x509 -new -nodes -key $CA_KEY -sha256 -days $DAYS_VALID_CA -out $CA_CERT -subj $CA_SUBJECT
if [ $? -ne 0 ]; then
  echo "Erreur lors de la génération du certificat du CA"
  exit 1
fi

# Générer la clé privée pour l'hôte
openssl genpkey -algorithm RSA -out $HOST_KEY -pkeyopt rsa_keygen_bits:2048
if [ $? -ne 0 ]; then
  echo "Erreur lors de la génération de la clé privée pour l'hôte"
  exit 1
fi

# Générer une requête de signature de certificat (CSR) pour l'hôte
openssl req -new -key $HOST_KEY -out $HOST_CSR -subj "/CN=$HOST_NAME"
if [ $? -ne 0 ]; then
  echo "Erreur lors de la génération de la requête de signature de certificat pour l'hôte"
  exit 1
fi


# Créer un fichier de configuration pour le sous-domaine
cat > $EXT_FILE <<EOL
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = $HOST_NAME
EOL

# Signer le CSR de l'hôte avec le certificat du CA pour générer le certificat de l'hôte
openssl x509 -req -in $HOST_CSR -CA $CA_CERT -CAkey $CA_KEY -CAcreateserial -out $HOST_CERT -days $DAYS_VALID_HOST -sha256 -extfile $EXT_FILE
if [ $? -ne 0 ]; then
  echo "Erreur lors de la signature de la requête de signature de certificat de l'hôte"
  exit 1
fi

# Convertir le certificat du CA au format DER pour l'embarquer sur cible
openssl x509 -outform der -in $CA_CERT -out $CA_CERT_DER
if [ $? -ne 0 ]; then
  echo "Erreur lors de la conversion du certificat de l'hôte au format DER"
  exit 1
fi

echo Certificat host
# Afficher les détails du certificat de l'hôte
openssl x509 -in $HOST_CERT -text -noout

echo Certificat CA
# Afficher les détails du certificat du CA
openssl x509 -in $CA_CERT -text -noout

rm -f host.ext
if [ $? -ne 0 ]; then
  echo "Erreur lors de la suppresion du fichier host.ext"
  exit 1
fi


echo "Certificat CA généré avec succès et sauvegardé dans $CA_CERT"
echo "Certificat de l'hôte généré avec succès et sauvegardé dans $HOST_CERT"
