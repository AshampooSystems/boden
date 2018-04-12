pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh '/data/boden-ci/docker-build.sh "${WORKSPACE}"'
            }
        }
    }
}