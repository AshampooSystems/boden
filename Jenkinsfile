pipeline {
    agent none
    stages {
        stage('Build Linux') {
        	agent {
        		label 'master'
        	}
            steps {
                sh '/data/boden-ci/docker-build.sh "${WORKSPACE}"'
            }
        }
    }
}