pipeline {
    agent none
    options {
        disableConcurrentBuilds()
    }
    stages {

        stage('Build Android') {
            agent {
                dockerfile {
                    filename 'Dockerfile_android'
                    additionalBuildArgs  '--build-arg ANDROID_API_VERSION=26 --build-arg ANDROID_BUILD_TOOLS_VERSION=26.0.2'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }

            steps {
                sh 'python build.py prepare --platform android --build-system AndroidStudio'
                sh 'python build.py build --platform android --config Debug'
            }
        }

        /* We have to figure out how we can detect when the tests are done.
           Then we need to kill the emulator and end the "run" process.
           We cannot activate this before that is done.
        stage('Test Android') {
            agent {
                dockerfile {
                    filename 'Dockerfile_android'
                    additionalBuildArgs  '--build-arg ANDROID_API_VERSION=26 --build-arg ANDROID_BUILD_TOOLS_VERSION=26.0.2'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                    reuseNode true
                }
            }

            steps {
                sh 'python build.py --platform android --config Debug --module testboden run'
                sh 'python build.py --platform android --config Debug --module testbodenui run'
                sh 'python build.py --platform android --config Debug --module testbodentiming run'
            }
        }*/

/*
        stage('Build Linux') {
        	agent {
        		label 'master'
        	}
            steps {
                sh '/data/boden-ci/boden-gtk-ubuntu/docker-build.sh "${WORKSPACE}" $GIT_BRANCH'
            }
        }

        stage('Test Linux') {
            agent {
                label 'master'
            }
            steps {
                sh '/data/boden-ci/boden-gtk-ubuntu/docker-test.sh "${WORKSPACE}" $GIT_BRANCH'
            }
        }*/
    }
}
