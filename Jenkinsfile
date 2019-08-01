pipeline {
    agent none

    options {
        timeout(time: 4, unit: 'HOURS')
        buildDiscarder(logRotator(numToKeepStr: '5', artifactNumToKeepStr: '5'))
    }

    environment {
        // Environment variables needed for the github-release tool
        GITHUB_TOKEN = credentials('github-boden-release-token')
        GITHUB_USER = 'AshampooSystems'
        GITHUB_REPO = 'boden'
    }

    stages {
        stage('Checks') {
            parallel {
                stage('Formatting') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        dockerfile {
                            filename 'docker/Dockerfile_android'
                            additionalBuildArgs  '-t boden_android'
                            label 'boden'
                        }
                    }
                    stages {
                        stage('Run clang-format') {
                            steps {
                                sh 'python boden.py build -a x86_64 --target FormatSources'
                            }
                        }
                        stage('Check for changes') {
                            steps {
                                script {
                                    List<String> sourceChanged = sh(returnStdout: true, script: "git diff --name-only").split()
                                    if(sourceChanged.size() > 0) {
                                        String changedFiles = "Some files were changed by clang-format, make sure to format before committing:\n";
                                        for (int i = 0; i < sourceChanged.size(); i++) {
                                            changedFiles += sourceChanged[i] + "\n";
                                            if(i > 10) {
                                                changedFiles += "...\n";
                                                break;
                                            }
                                        }
                                        println changedFiles
                                        error(changedFiles)
                                    }
                                }
                            }
                        }
                    }
                }

                stage('mkdocs') {
                    agent {
                        dockerfile {
                            filename 'docker/Dockerfile_mkdocs'
                            label 'boden-general'
                            additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g)'
                        }
                    }

                    steps {
                        sh '/usr/bin/python3 -m pip install -r docs/requirements.txt'
                        sh 'cd docs && /usr/bin/python3 check-docs.py'
                    }
                }
            }
        }

        stage('Platforms') {
            parallel {
                stage('Android - Linux') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        dockerfile {
                            filename 'docker/Dockerfile_android'
                            additionalBuildArgs  '-t boden_android'
                            label 'boden'
                        }
                    }
                    stages {
                        stage('Build') {
                            steps {
                                sh 'python boden.py prepare -b make -a arm64-v8a'
                                sh 'python boden.py prepare -b make -a x86_64'

                                // This is only necessary for testing, move out once tests work again
                                sh 'python boden.py prepare -a x86_64 -b AndroidStudio'

                                sh 'python boden.py build -b AndroidStudio $BUILD_EXTRA_ARGS'

                                stash includes: 'build/android/**/*', name: 'android-build-data'
                            }
                        }
                        /*stage('Package') {
                            steps {
                                sh 'python boden.py package -p android -b AndroidStudio $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'android-packages'
                            }
                        }*/
                        /*stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python boden.py run --target testboden -- --out testresults/android_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python boden.py run --target testbodenui -- --out testresults/android_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }*/
                        stage('Test boden new') {
                            steps {
                                sh 'python boden.py new -n testapp && cd testapp && python ../boden.py build -p android'
                            }
                        }                        
                    }
                }

                /*
                // Note: this is separate because we need to use a separate
                // docker image as the basis. A docker that can build and test
                // android apps reaches the limits for the maximum container size
                // that AWS ECS Fargate allows.
                stage('Android Test') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-test-android'
                    }
                    stages {
                        stage('Test') {
                            steps {
                                unstash 'android-build-data'

                                sh 'mkdir -p testresults'

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python boden.py run --target testboden -- --out testresults/android_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python boden.py run --target testbodenui -- --out testresults/android_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }

                    }
                }*/

                stage('IOS') {
                    environment {
                        BAUER_PLATFORM = 'ios'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                        BAUER_IDENTITY = 'iPhone Developer'
                    }
                    agent { label 'iOS' }
                    stages {
                        stage('Build') {
                            steps {
                                sh 'python boden.py build'
                            }
                        }

                        stage('Package') {
                            steps {
                                sh 'python boden.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'ios-packages'
                            }
                        }

                        stage('Sign') {
                            environment {
                                JENKINS_USER = credentials('mac-jenkins-user')
                                BAUER_KEYCHAIN = credentials('jenkins-dev-certificate-keychain')
                            }
                            steps {
                                sh 'python boden.py codesign --password $JENKINS_USER_PSW'
                            }
                        }

                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python boden.py run --target testBoden -- --gtest_output=xml:$PWD/testresults/ios_testBoden.xml'
                                sh 'cat $PWD/testresults/ios_testBoden.xml'
                                junit "testresults/ios_testBoden.xml"

                                archiveArtifacts artifacts: 'testresults/ios_testBoden.xml'
                            }
                        }

                        stage('Test boden new') {
                            steps {
                                sh 'python boden.py new -n testapp && cd testapp && python ../boden.py build -p ios'
                            }
                        }
                    }
                }
                stage('Android - Windows') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent { label 'boden-win' }
                    stages {
                        stage('Build') {
                            steps {
                                bat 'python boden.py prepare -a x86_64'
                                //bat 'python boden.py prepare -a arm64-v8a'
                                bat 'python boden.py build -a x86_64 %BUILD_EXTRA_ARGS%'
                                //bat 'python boden.py build -a arm64-v8a %BUILD_EXTRA_ARGS%'
                            }
                        }
                        stage('Test boden new') {
                            steps {
                                bat 'python boden.py new -n testapp && cd testapp && python ../boden.py build -a x86_64'
                            }
                        }                        
                    }
                }  
            }
        }

        stage('Deployment') {
            parallel {
                stage('mkdocs') {
                    when {
                        branch "master"
                    }
                    agent {
                        dockerfile {
                            filename 'docker/Dockerfile_mkdocs'
                            label 'boden-general'
                            additionalBuildArgs '--build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g)'
                        }
                    }

                    steps {
                        withCredentials([sshUserPrivateKey(credentialsId: 'deploy-boden', keyFileVariable: 'SSH_KEY', usernameVariable: 'SSH_USER')]) {
                            sh 'git remote add documentation git@github.com:AshampooSystems/boden-private.git || true'
                            sh '/usr/bin/python3 -m pip install -r docs/requirements.txt'

                            withEnv(["GIT_SSH_COMMAND=ssh -o StrictHostKeyChecking=no -o User=${SSH_USER} -i ${SSH_KEY}"]) {
                                sh 'cd docs && /usr/bin/python3 -m mkdocs gh-deploy --force -c --remote-name documentation'
                            }
                        }
                    }
                }

                stage('Release') {
                    when {
                        branch "release/*"
                    }
                    agent {
                        dockerfile {
                            filename 'docker/Dockerfile_github'
                            args '--volume ${WORKSPACE}:/boden'
                            label 'boden-general'
                        }
                    }

                    steps {
                        /*unstash 'android-packages'*/
                        unstash 'ios-packages'
                        unstash 'macos-packages'
                        unstash 'linux-packages'

                        script {
                            env.RELEASE_NAME = env.BRANCH_NAME.split('/')[1]
                        }

                        sh 'github-release delete --tag $RELEASE_NAME || true'
                        sh 'github-release release --target $GIT_COMMIT --tag $RELEASE_NAME --name "boden - $RELEASE_NAME"'
                        sh 'ls /boden/build/package'
                        sh 'cd build/package && find . -exec sh -c \'github-release upload --tag $RELEASE_NAME --name $(basename $0) --file $0\' {} ";"'
                    }
                }
            }
        }
    }
    post {
        failure {
            slackSend (color: '#FF0000', channel: "boden-ci", message: "@here: FAILED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")
        }
    }
}
